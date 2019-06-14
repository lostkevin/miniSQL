#pragma once
#include <map>
#include "..\miniSQL\Commonheader.h"
#include "..\BPTree\BPTree_V3.h"
#include "..\BufferManager\BufferManager.h"

enum TreeTYPE {
	UNDEF,
	INT,
	FLOAT,
	STRING
};

static struct BasicInfo {
	TreeTYPE type;
	uint KeySize;
	uint order;

	BasicInfo () {
		type = UNDEF;
		KeySize = 0;
		order = 0;
	}
};

//保证每个node占用一个disk page，增加IO速度
class IndexManager {
private:
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
		void drop ();
		void close ();
		//打开一个文件，若文件存在，返回true，否则返回false
		bool open (const string& fileName);
	};

	//对于Index而言文件及文件头一定存在，不抛任何异常，运行时报错再说
	template<typename _KTy>
	class Index {
		//Root指针
		BPlusNode<_KTy> *Root;
		map<IndexInfo, BPlusNode<_KTy>> nodesActive;
		BasicInfo headerInfo;
		BufferIO & IOManager;
		//将索引更新到buffer,释放所有node
		void update () {
			for (auto iter = nodesActive.begin (); iter != nodesActive.end (); iter++) {
				if (iter->second.getDirtyState ()) {
					BYTE rawData[PAGE_SIZE] = { 0 };
					saveNode (iter->second, rawData);
					SaveRawData (iter->first, rawData);
				}
			}
			Root = nullptr;
			nodesActive.clear ();
		}
		//将BYTE*的数据转换成node
		BPlusNode<_KTy> getNode (const BYTE (&rawData)[PAGE_SIZE]) {

		}

		void saveNode (const BPlusNode<_KTy>, const BYTE (&rawData)[PAGE_SIZE]) {

		}
		BPlusNode<_KTy>* GetNodePtr (const IndexInfo& info) {
			auto iter = nodesActive.find (info);
			if (iter != nodesActive.end ()) {
				return &(iter->second);
			}
			else {
				BYTE rawData[PAGE_SIZE] = { 0 };
				if (ReadRawData (info, rawData)) {
					nodesActive[info] = getNode (rawData);
					return &nodesActive[info];
				}
				else return nullptr;
			}
		}
		//获取一个新Node，要求索引文件头已定义, 返回这个node的索引
		const IndexInfo GetNewNode ()  {
			getHeader ();
			//申请空间
			IndexInfo newNodeInfo = NewPage ();
			//绑定Info，初始化Node,这个node是dirty的
			nodesActive[newNodeInfo] = InitialNode ();
		}
		BPlusNode<_KTy> InitialNode () {			
			return BPlusNode<_KTy> (headerInfo.order, GetNodePtr, GetNewNode, true);
		}
		//设置根节点信息, 将根节点设置为info指向的节点
		void setRootInfo (const IndexInfo &info) {
			BYTE header[PAGE_SIZE];
			if (ReadRawData (IndexInfo (), header)) {
				BYTE* ptr = header;
				ptr += 16 + sizeof(TreeTYPE) + sizeof(uint) + sizeof(uint);
				*(IndexInfo*)ptr = info;
				SaveRawData (IndexInfo (), header);
			}
		}
		void getHeader () {
			if (headerInfo.type == UNDEF) {
				BYTE header[PAGE_SIZE];
				ReadRawData (IndexInfo (), header);
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
		//访问器，获取根节点索引, 并将Root加载到内存中
		//加载失败的话说明树已定义但根不存在，抛异常
		const IndexInfo getRootInfo () {

		}
		//将info指向的节点加入到freelist中
		void dropNode (IndexInfo &info) {
			auto iter = nodesActive.find (info);
			if (iter != nodesActive.end ()) {
				//删除该node
				nodesActive.erase (iter);
			}
			AddFreeList (info);
		}
	public:
		Index (BufferIO& bufferMgr) :IOManager (bufferMgr) {

		}
		const vector<IndexInfo> find (_KTy min, _KTy max) {

		 }
		const IndexInfo find (_KTy key) {

		 }
		//修改索引的相关方法
		//插入,用于建立新index,若key重复将会更新索引
		void insert (const _KTy &key, const IndexInfo & data) {
			Root->insert (key, data);
			//如果root有rsibling时，新建一个节点作为根，将root及其Rsibling作为新根的子节点
			if (Root->RIndex) {
				IndexInfo nullNode = GetNewNode ();
				BPlusNode<_KTy>* tmp = GetNodePtr (nullNode);
				//当分裂root时，向根插入的index保存在原根的max指针对应的key中
				tmp->Index[0].key = Root->Index[Root->getSize ()].key;
				//清空max指针的key
				Root->Index[Root->getSize ()].key = _KTy ();
				//设置子节点索引
				tmp->Index[0].info = getRootInfo ();
				tmp->Index[1].info = Root->RIndex;
				//设置父节点索引
				Root->Parent = nullNode;
				GetNodePtr (Root->RIndex)->Parent = nullNode;
				//更新根节点信息
				setRootInfo (nullNode);
				//更新根指针
				Root = tmp;
			}
			update ();
		}
		//删除，若key不存在返回false, 删除成功返回true,否则抛异常
		bool erase (const _KTy &key) {
			bool flag = Root->erase (key);
			if (!flag)return false;
			//删除后可能使root的size=1，此时释放原来的根节点并更新根的位置
			if (Root->getSize () == 1) {

			}
		}
		//drop, 令root = nullptr 即可
		void dropIndex () {
#ifdef DEBUG
			if (nodesActive.size () > 1) throw new exception ("Nodes aren't update");
#endif
			update ();
			Root = nullptr;
		}
	};



	//根据KeySize计算一个node的order
	static uint calcOrder (uint KeySize = sizeof(int));
public:
	//1个IndexManager实例管理一个索引文件
	union {
		Index<int> *ITree;
		Index<float> *FTree;
		Index<string> *CTree;
	};
	bool fail ();
	BufferIO IOManager;
	TreeTYPE type;

	//设置索引基本信息，keyType = string时keysize = size()
	bool setIndexInfo (TreeTYPE type, uint keySize = 4);
	IndexManager (BufferManager& bufferMgr);
	IndexManager (const string &fileName, BufferManager& bufferMgr);
	void dropIndex ();
	~IndexManager ();
	//打开已有文件/新建文件，文件不能正确读取时返回false
	bool open (const string& fileName);
	void close ();

	template<typename _Ty> const vector<IndexInfo> find (_Ty min, _Ty max);
	template<typename _Ty> const IndexInfo find (_Ty key);
	//修改索引的相关方法
	//插入,用于建立新index,若key重复将会更新索引，这里将会抛出异常
	template<typename _Ty> void insert (const _Ty &key, const IndexInfo & data);
	//删除，若key不存在返回false, 删除成功返回true,否则抛异常
	template<typename _Ty> bool erase (const _Ty &key);
};


