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

//��֤ÿ��nodeռ��һ��disk page������IO�ٶ�
class IndexManager {
private:
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
		void drop ();
		void close ();
		//��һ���ļ������ļ����ڣ�����true�����򷵻�false
		bool open (const string& fileName);
	};

	//����Index�����ļ����ļ�ͷһ�����ڣ������κ��쳣������ʱ������˵
	template<typename _KTy>
	class Index {
		//Rootָ��
		BPlusNode<_KTy> *Root;
		map<IndexInfo, BPlusNode<_KTy>> nodesActive;
		BasicInfo headerInfo;
		BufferIO & IOManager;
		//���������µ�buffer,�ͷ�����node
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
		//��BYTE*������ת����node
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
		//��ȡһ����Node��Ҫ�������ļ�ͷ�Ѷ���, �������node������
		const IndexInfo GetNewNode ()  {
			getHeader ();
			//����ռ�
			IndexInfo newNodeInfo = NewPage ();
			//��Info����ʼ��Node,���node��dirty��
			nodesActive[newNodeInfo] = InitialNode ();
		}
		BPlusNode<_KTy> InitialNode () {			
			return BPlusNode<_KTy> (headerInfo.order, GetNodePtr, GetNewNode, true);
		}
		//���ø��ڵ���Ϣ, �����ڵ�����Ϊinfoָ��Ľڵ�
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
		//����������ȡ���ڵ�����, ����Root���ص��ڴ���
		//����ʧ�ܵĻ�˵�����Ѷ��嵫�������ڣ����쳣
		const IndexInfo getRootInfo () {

		}
		//��infoָ��Ľڵ���뵽freelist��
		void dropNode (IndexInfo &info) {
			auto iter = nodesActive.find (info);
			if (iter != nodesActive.end ()) {
				//ɾ����node
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
		//�޸���������ط���
		//����,���ڽ�����index,��key�ظ������������
		void insert (const _KTy &key, const IndexInfo & data) {
			Root->insert (key, data);
			//���root��rsiblingʱ���½�һ���ڵ���Ϊ������root����Rsibling��Ϊ�¸����ӽڵ�
			if (Root->RIndex) {
				IndexInfo nullNode = GetNewNode ();
				BPlusNode<_KTy>* tmp = GetNodePtr (nullNode);
				//������rootʱ����������index������ԭ����maxָ���Ӧ��key��
				tmp->Index[0].key = Root->Index[Root->getSize ()].key;
				//���maxָ���key
				Root->Index[Root->getSize ()].key = _KTy ();
				//�����ӽڵ�����
				tmp->Index[0].info = getRootInfo ();
				tmp->Index[1].info = Root->RIndex;
				//���ø��ڵ�����
				Root->Parent = nullNode;
				GetNodePtr (Root->RIndex)->Parent = nullNode;
				//���¸��ڵ���Ϣ
				setRootInfo (nullNode);
				//���¸�ָ��
				Root = tmp;
			}
			update ();
		}
		//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
		bool erase (const _KTy &key) {
			bool flag = Root->erase (key);
			if (!flag)return false;
			//ɾ�������ʹroot��size=1����ʱ�ͷ�ԭ���ĸ��ڵ㲢���¸���λ��
			if (Root->getSize () == 1) {

			}
		}
		//drop, ��root = nullptr ����
		void dropIndex () {
#ifdef DEBUG
			if (nodesActive.size () > 1) throw new exception ("Nodes aren't update");
#endif
			update ();
			Root = nullptr;
		}
	};



	//����KeySize����һ��node��order
	static uint calcOrder (uint KeySize = sizeof(int));
public:
	//1��IndexManagerʵ������һ�������ļ�
	union {
		Index<int> *ITree;
		Index<float> *FTree;
		Index<string> *CTree;
	};
	bool fail ();
	BufferIO IOManager;
	TreeTYPE type;

	//��������������Ϣ��keyType = stringʱkeysize = size()
	bool setIndexInfo (TreeTYPE type, uint keySize = 4);
	IndexManager (BufferManager& bufferMgr);
	IndexManager (const string &fileName, BufferManager& bufferMgr);
	void dropIndex ();
	~IndexManager ();
	//�������ļ�/�½��ļ����ļ�������ȷ��ȡʱ����false
	bool open (const string& fileName);
	void close ();

	template<typename _Ty> const vector<IndexInfo> find (_Ty min, _Ty max);
	template<typename _Ty> const IndexInfo find (_Ty key);
	//�޸���������ط���
	//����,���ڽ�����index,��key�ظ�����������������ｫ���׳��쳣
	template<typename _Ty> void insert (const _Ty &key, const IndexInfo & data);
	//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
	template<typename _Ty> bool erase (const _Ty &key);
};


