#pragma once
#define DEBUG
#include <map>
#include <vector>
#include "..\BPTree\BPTree_V3.h"
#include "..\BufferManager\BufferManager.h"



struct BasicInfo {
	TreeTYPE type;
	uint KeySize;
	uint order;

	BasicInfo () {
		type = UNDEF;
		KeySize = 0;
		order = 0;
	}
};

//这个类对Buffer的读写进一步封装
class BufferIO {
	string fileName;
	BufferManager& bufferMgr;
public:
	BufferIO (const string& fileName, BufferManager& bufferMgr) :fileName (fileName), bufferMgr (bufferMgr) {

	}
	BufferIO (BufferManager& bufferMgr) :bufferMgr (bufferMgr) {

	}
	bool ReadRawData (const IndexInfo&, BYTE (&rawData)[PAGE_SIZE]);
	bool WriteRawData (const IndexInfo&, const BYTE (&rawData)[PAGE_SIZE]);
	const IndexInfo NewPage ();
	void erase (const IndexInfo &);
	void drop ();
	void close ();
	//打开一个文件，若文件存在，返回true，否则返回false
	bool open (const string& fileName);
	//手动释放资源
	void release (const IndexInfo&);
};

//对于Index而言文件及文件头一定存在，不抛任何异常，运行时报错再说
template<typename _KTy>
class Index {
	//储存当前活动的节点
	map<IndexInfo, BPlusNode<_KTy>> nodesActive;
	BasicInfo headerInfo;
	BufferIO & IOManager;
	//将索引更新到buffer,释放所有node
	void update () {
		for (auto iter = nodesActive.begin (); iter != nodesActive.end (); iter++) {
			if (iter->second.getDirtyState ()) {
				BYTE rawData[PAGE_SIZE] = { 0 };
				saveNode (iter->second, rawData);
				IOManager.WriteRawData (iter->first, rawData);
			}
			else IOManager.release (iter->first);
		}
		nodesActive.clear ();
	}
	//将BYTE*的数据转换成node,得到的node不应是左值
	const BPlusNode<_KTy> ToNode (const BYTE (&rawData)[PAGE_SIZE]) {
		BPlusNode<_KTy> tmp = InitialNode();
		const BYTE* ptr = rawData;
		tmp.Parent = *(IndexInfo*)ptr;
		ptr += sizeof (IndexInfo);
		tmp.LIndex = *(IndexInfo*)ptr;
		ptr += sizeof (IndexInfo);
		tmp.RIndex = *(IndexInfo*)ptr;
		ptr += sizeof (IndexInfo);
		uint PairSize = *(uint*)ptr;
		ptr += sizeof (uint);
		tmp.size = *(uint*)ptr;
		ptr += sizeof (uint);
		tmp.type = *(NodeType*)ptr;
		ptr = rawData;
		for (uint i = 0; i < tmp.size; i++) {	
			tmp.index[i] = *(Pair<_KTy>*)(ptr + 0x100 + i * PairSize);
		}
		return tmp;
	}

	void saveNode (const BPlusNode<_KTy>& Node, BYTE (&rawData)[PAGE_SIZE]) {
		BYTE* ptr = rawData;
		*(IndexInfo*)ptr = Node.Parent;
		ptr += sizeof (IndexInfo);
		*(IndexInfo*)ptr = Node.LIndex;
		ptr += sizeof (IndexInfo);
		*(IndexInfo*)ptr = Node.RIndex;
		ptr += sizeof (IndexInfo);
		*(uint*)ptr = sizeof (Pair<_KTy>);
		ptr += sizeof (uint);
		*(uint*)ptr = Node.size;
		ptr += sizeof (uint);
		*(NodeType*)ptr = Node.type;
		ptr = rawData;
		for (uint i = 0; i < Node.size; i++) {
			*(Pair<_KTy>*)(ptr + 0x100 + i * sizeof (Pair<_KTy>)) = Node.index[i];
		}
	}

	//将info指向的node加入到活动节点列表，并返回一个指向这个node的指针
	BPlusNode<_KTy>* GetNodePtr (const IndexInfo& info) {
		if (!info._fileOffset)return nullptr;
		auto iter = nodesActive.find (info);
		if (iter != nodesActive.end ()) {
			return &(iter->second);
		}
		else {
			BYTE rawData[PAGE_SIZE] = { 0 };
			if (IOManager.ReadRawData (info, rawData)) {
				nodesActive[info] = ToNode (rawData);
				nodesActive[info].thisPos = info;
				return &nodesActive[info];
			}
			else return nullptr;
		}
	}

	//获取一个新Node，要求索引文件头已定义, 返回这个node的索引
	const IndexInfo GetNewNode () {
		getHeader ();
		//申请空间
		IndexInfo newNodeInfo = IOManager.NewPage ();
		//绑定Info，初始化Node,这个node是dirty的
		nodesActive[newNodeInfo] = InitialNode ();
		nodesActive[newNodeInfo].thisPos = newNodeInfo;
		return newNodeInfo;
	}

	BPlusNode<_KTy> InitialNode () {
		getHeader ();
		BPlusNode<_KTy> tmp (headerInfo.order, this, true);
		return tmp;
	}

	//设置根节点信息, 将根节点设置为info指向的节点
	void setRootInfo (const IndexInfo &info) {
		BYTE header[PAGE_SIZE];
		if (IOManager.ReadRawData (IndexInfo (), header)) {
			BYTE* ptr = header;
			ptr += 16 + sizeof (TreeTYPE) + sizeof (uint) + sizeof (uint);
			*(IndexInfo*)ptr = info;
			IOManager.WriteRawData (IndexInfo (), header);
		}
	}

	void getHeader () {
		if (headerInfo.type == UNDEF) {
			BYTE header[PAGE_SIZE];
			IOManager.ReadRawData (IndexInfo (), header);
			BYTE* ptr = header;
			ptr += 16;
			headerInfo.type = *(TreeTYPE*)ptr;
			ptr += sizeof (TreeTYPE);
			headerInfo.KeySize = *(uint*)ptr;
			ptr += sizeof (uint);
			headerInfo.order = *(uint*)ptr;
			if (headerInfo.type == UNDEF || !headerInfo.order || !headerInfo.KeySize)throw new exception ("Incorrect Header!");
		}
	}

	//访问器，获取根节点索引
	//加载失败的话将导致一个未定义的索引
	const IndexInfo getRootInfo () {
		BYTE header[PAGE_SIZE];
		IOManager.ReadRawData (IndexInfo (), header);
		BYTE* ptr = header;
		ptr += 16 + sizeof (TreeTYPE) + sizeof (uint) + sizeof (uint);
		IndexInfo rootInfo = *(IndexInfo*)ptr;
		return rootInfo;
	}

	//将info指向的节点加入到freelist中
	void dropNode (IndexInfo &info) {
		IOManager.erase (info);
	}
	friend class BPlusNode<_KTy>;
#ifdef DEBUG
	uint examineNode (const IndexInfo &node) {
		//检查当前节点数据是否正确
		uint nodesize = 1;
		BPlusNode<_KTy>* ptr = GetNodePtr (node);
		if (ptr->type == LEAF)return nodesize;
		else if (ptr->type != NONLEAF)throw new exception ();
		for (uint i = 0; i < ptr->size; i++) {
			nodesize += examineNode (ptr->index[i].info);
		}
		return nodesize;
	}
#endif // DEBUG

public:
	Index (BufferIO& bufferMgr) :IOManager (bufferMgr) {
	}

	//搜索指定范围的key的indexinfo，结果数=vector.size()
	const vector<IndexInfo> find (_KTy min, _KTy max) {
		IndexInfo rootInfo = getRootInfo ();
		BPlusNode<_KTy> *Root = GetNodePtr (rootInfo);
		vector<IndexInfo> result;
		if (Root) {
			Root->find (min, max, result);
		}
		update ();
		return result;
	}

	void getAllIndex (vector<IndexInfo> & result) {
		IndexInfo rootInfo = getRootInfo ();
		BPlusNode<_KTy> *Root = GetNodePtr (rootInfo);
		while (Root && Root->type != LEAF)Root = GetNodePtr (Root->index[0].info);
		while (Root) {
			for (uint i = 0; i < Root->size; i++) {
				result.push_back (Root->index[i].info);
			}
			Root = GetNodePtr (Root->RIndex);
		}
		update ();
	}

	//搜索指定key的indexinfo，如果找不到，返回文件头的indexinfo
	const IndexInfo find (_KTy key) {
		IndexInfo rootInfo = getRootInfo ();
		BPlusNode<_KTy> *Root = GetNodePtr (rootInfo);
		if (Root) {
			IndexInfo tmp = Root->find (key);
			update ();
			return tmp;
		}
		return IndexInfo ();
	}

	//修改索引的相关方法
	//插入,用于建立新index,若key重复将会更新索引
	void insert (const _KTy &key, const IndexInfo & data) {
		IndexInfo rootInfo = getRootInfo ();
		BPlusNode<_KTy> *Root = GetNodePtr (rootInfo);
		if (!Root) {
			IndexInfo nullNode = GetNewNode ();
			Root = GetNodePtr (nullNode);
			Root->index[0].key = key;
			Root->index[0].info = data;
			setRootInfo (nullNode);
			Root->insert (key, data);
		}
		else {
			Root->Search (key)->insert (key, data);
		}
		//如果root有rsibling时，新建一个节点作为根，将root及其Rsibling作为新根的子节点
		if (GetNodePtr(Root->RIndex)) {
			IndexInfo nullNode = GetNewNode ();
			BPlusNode<_KTy>* tmp = GetNodePtr (nullNode);
			//当分裂root时，向根插入的index保存在原根的max指针对应的key中
			tmp->index[0].key = Root->index[Root->size].key;
			//清空max指针的key
			Root->index[Root->size].key = _KTy ();
			//设置子节点索引
			tmp->index[0].info = getRootInfo ();
			tmp->index[1].info = Root->RIndex;
			//设置父节点索引
			Root->Parent = nullNode;
			GetNodePtr (Root->RIndex)->Parent = nullNode;
			//更新根节点信息
			setRootInfo (nullNode);
		}
		update ();
	}
	//删除，若key不存在返回false, 删除成功返回true,否则抛异常
	bool erase (const _KTy &key) {
		IndexInfo rootInfo = getRootInfo ();
		BPlusNode<_KTy> *Root = GetNodePtr (rootInfo);
		if (!Root)return false;
		bool flag = Root->erase (key);
		if (!flag)return false;
		//删除后可能使root的size=1，此时释放原来的根节点并更新根的位置
		if (Root->size == 1) {
			setRootInfo (Root->index[0].info);
			IOManager.erase (rootInfo);
		}
		else if (!Root->size) {
			setRootInfo (IndexInfo ());
			IOManager.erase (rootInfo);
		}
		return true;
	}

#ifdef DEBUG
	uint examine () {
		IndexInfo root = getRootInfo ();
		return examineNode (root);
	}
#endif // DEBUG

};

//保证每个node占用一个disk page，增加IO速度
class IndexManager {
private:
	//根据KeySize计算一个node的order
	static uint calcOrder (uint KeySize = sizeof(int));
	union {
		Index<int> *ITree;
		Index<float> *FTree;
		Index<string> *CTree;
	};
	bool fail ();
	BufferIO IOManager;
	TreeTYPE type;
public:
	//设置索引基本信息，keyType = string时keysize = size()
	bool setIndexInfo (TreeTYPE type, uint keySize = 4);
	IndexManager (BufferManager& bufferMgr);
	IndexManager (const string &fileName, BufferManager& bufferMgr);
	void dropIndex ();
	~IndexManager ();
	//打开已有文件/新建文件，文件不能正确读取时返回false
	bool open (const string& fileName);
	void close ();

	template<typename _KTy> const vector<IndexInfo> find (_KTy min, _KTy max);
	template<typename _KTy> const IndexInfo find (_KTy key);
	//修改索引的相关方法
	//插入,用于建立新index,若key重复将会更新索引，这里将会抛出异常
	template<typename _KTy> void insert (const _KTy &key, const IndexInfo & data){}
	//删除，若key不存在返回false, 删除成功返回true,否则抛异常
	template<typename _KTy> bool erase (const _KTy &key);
	//获取所有索引
	void getAllIndex (vector<IndexInfo> & result);

#ifdef DEBUG
	uint examine () {
		//检查文件是否完好，并返回文件中的节点数
		if (setIndexInfo (INT))return 0;
		switch (type) {
		case INT: {
			return ITree->examine ();
		}
		case FLOAT: {
			return FTree->examine ();
		}
		case STRING: {
			return CTree->examine ();
		}
		}
		return 0;
	}
#endif // DEBUG

};



template<typename _KTy>
inline const IndexInfo IndexManager::find (_KTy key)
{
	throw new exception ("NullIndexException");
}

template<>
inline const IndexInfo IndexManager::find (int key)
{
	switch (type) {
	case INT: {
		return ITree->find (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline const IndexInfo IndexManager::find (float key)
{
	switch (type) {
	case FLOAT: {
		return FTree->find (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline const IndexInfo IndexManager::find (string key)
{
	switch (type) {
	case INT: {
		return CTree->find (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<typename _KTy>
inline const vector<IndexInfo> IndexManager::find (_KTy min, _KTy max)
{
	switch (type) {
	case INT: {
		static_assert(!Conversion<_KTy, int>::state, "Key Type Incorrect!");
		return ITree->find (min, max);
	}
	case FLOAT: {
		static_assert(!Conversion<_KTy, float>::state, "Key Type Incorrect!");
		return FTree->find (min, max);
	}
	case STRING: {
		static_assert(!Conversion<_KTy, string>::state, "Key Type Incorrect!");
		return CTree->find (min, max);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline void IndexManager::insert (const int &key, const IndexInfo & data)
{
	switch (type) {
	case INT: {
		ITree->insert ((int)key, data);
		break;
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline void IndexManager::insert (const float &key, const IndexInfo & data)
{
	switch (type) {
	case FLOAT: {
		FTree->insert ((float)key, data);
		break;
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline void IndexManager::insert (const string &key, const IndexInfo & data)
{
	switch (type) {
	case STRING: {
		CTree->insert (key, data);
		break;
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<typename _KTy>
inline bool IndexManager::erase (const _KTy & key)
{
	throw new exception ("NullIndexException");
}

template<>
inline bool IndexManager::erase (const int & key)
{
	switch (type) {
	case INT: {
		return ITree->erase (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline bool IndexManager::erase (const float & key)
{
	switch (type) {
	case FLOAT: {
		return FTree->erase (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<>
inline bool IndexManager::erase (const string & key)
{
	switch (type) {
	case STRING: {
		return CTree->erase (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}