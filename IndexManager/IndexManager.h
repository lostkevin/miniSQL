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

//������Buffer�Ķ�д��һ����װ
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
	//��һ���ļ������ļ����ڣ�����true�����򷵻�false
	bool open (const string& fileName);
	//�ֶ��ͷ���Դ
	void release (const IndexInfo&);
};

//����Index�����ļ����ļ�ͷһ�����ڣ������κ��쳣������ʱ������˵
template<typename _KTy>
class Index {
	//���浱ǰ��Ľڵ�
	map<IndexInfo, BPlusNode<_KTy>> nodesActive;
	BasicInfo headerInfo;
	BufferIO & IOManager;
	//���������µ�buffer,�ͷ�����node
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
	//��BYTE*������ת����node,�õ���node��Ӧ����ֵ
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

	//��infoָ���node���뵽��ڵ��б�������һ��ָ�����node��ָ��
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

	//��ȡһ����Node��Ҫ�������ļ�ͷ�Ѷ���, �������node������
	const IndexInfo GetNewNode () {
		getHeader ();
		//����ռ�
		IndexInfo newNodeInfo = IOManager.NewPage ();
		//��Info����ʼ��Node,���node��dirty��
		nodesActive[newNodeInfo] = InitialNode ();
		nodesActive[newNodeInfo].thisPos = newNodeInfo;
		return newNodeInfo;
	}

	BPlusNode<_KTy> InitialNode () {
		getHeader ();
		BPlusNode<_KTy> tmp (headerInfo.order, this, true);
		return tmp;
	}

	//���ø��ڵ���Ϣ, �����ڵ�����Ϊinfoָ��Ľڵ�
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

	//����������ȡ���ڵ�����
	//����ʧ�ܵĻ�������һ��δ���������
	const IndexInfo getRootInfo () {
		BYTE header[PAGE_SIZE];
		IOManager.ReadRawData (IndexInfo (), header);
		BYTE* ptr = header;
		ptr += 16 + sizeof (TreeTYPE) + sizeof (uint) + sizeof (uint);
		IndexInfo rootInfo = *(IndexInfo*)ptr;
		return rootInfo;
	}

	//��infoָ��Ľڵ���뵽freelist��
	void dropNode (IndexInfo &info) {
		IOManager.erase (info);
	}
	friend class BPlusNode<_KTy>;
#ifdef DEBUG
	uint examineNode (const IndexInfo &node) {
		//��鵱ǰ�ڵ������Ƿ���ȷ
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

	//����ָ����Χ��key��indexinfo�������=vector.size()
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

	//����ָ��key��indexinfo������Ҳ����������ļ�ͷ��indexinfo
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

	//�޸���������ط���
	//����,���ڽ�����index,��key�ظ������������
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
		//���root��rsiblingʱ���½�һ���ڵ���Ϊ������root����Rsibling��Ϊ�¸����ӽڵ�
		if (GetNodePtr(Root->RIndex)) {
			IndexInfo nullNode = GetNewNode ();
			BPlusNode<_KTy>* tmp = GetNodePtr (nullNode);
			//������rootʱ����������index������ԭ����maxָ���Ӧ��key��
			tmp->index[0].key = Root->index[Root->size].key;
			//���maxָ���key
			Root->index[Root->size].key = _KTy ();
			//�����ӽڵ�����
			tmp->index[0].info = getRootInfo ();
			tmp->index[1].info = Root->RIndex;
			//���ø��ڵ�����
			Root->Parent = nullNode;
			GetNodePtr (Root->RIndex)->Parent = nullNode;
			//���¸��ڵ���Ϣ
			setRootInfo (nullNode);
		}
		update ();
	}
	//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
	bool erase (const _KTy &key) {
		IndexInfo rootInfo = getRootInfo ();
		BPlusNode<_KTy> *Root = GetNodePtr (rootInfo);
		if (!Root)return false;
		bool flag = Root->erase (key);
		if (!flag)return false;
		//ɾ�������ʹroot��size=1����ʱ�ͷ�ԭ���ĸ��ڵ㲢���¸���λ��
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

//��֤ÿ��nodeռ��һ��disk page������IO�ٶ�
class IndexManager {
private:
	//����KeySize����һ��node��order
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
	//��������������Ϣ��keyType = stringʱkeysize = size()
	bool setIndexInfo (TreeTYPE type, uint keySize = 4);
	IndexManager (BufferManager& bufferMgr);
	IndexManager (const string &fileName, BufferManager& bufferMgr);
	void dropIndex ();
	~IndexManager ();
	//�������ļ�/�½��ļ����ļ�������ȷ��ȡʱ����false
	bool open (const string& fileName);
	void close ();

	template<typename _KTy> const vector<IndexInfo> find (_KTy min, _KTy max);
	template<typename _KTy> const IndexInfo find (_KTy key);
	//�޸���������ط���
	//����,���ڽ�����index,��key�ظ�����������������ｫ���׳��쳣
	template<typename _KTy> void insert (const _KTy &key, const IndexInfo & data){}
	//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
	template<typename _KTy> bool erase (const _KTy &key);
	//��ȡ��������
	void getAllIndex (vector<IndexInfo> & result);

#ifdef DEBUG
	uint examine () {
		//����ļ��Ƿ���ã��������ļ��еĽڵ���
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