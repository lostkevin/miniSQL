#pragma once
#include <map>
#include <vector>
#include "..\BPTree\BPTree_V3.h"
#include "..\BufferManager\BufferManager.h"

enum TreeTYPE {
	UNDEF,
	INT,
	FLOAT,
	STRING
};

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
		tmp.Index = new IndexInfo[tmp.getOrder () + 1];
		for (uint i = 0; i < tmp.size; i++) {	
			tmp.Index[i] = *(Pair<_KTy>*)(ptr + 0x100 + i * PairSize);
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
		*(uint*)ptr = sizeof (Pair);
		ptr += sizeof (uint);
		*(uint*)ptr = Node.size;
		ptr += sizeof (uint);
		*(NodeType*)ptr = Node.type;
		ptr = rawData;
		for (uint i = 0; i < Node.size; i++) {
			*(Pair<_KTy>*)Node = Node.Index[i];
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
	}

	BPlusNode<_KTy> InitialNode () {
		getHeader ();
		return BPlusNode<_KTy> (headerInfo.order, GetNodePtr, GetNewNode, true);
	}

	//���ø��ڵ���Ϣ, �����ڵ�����Ϊinfoָ��Ľڵ�
	void setRootInfo (const IndexInfo &info) {
		BYTE header[PAGE_SIZE];
		if (IOManager.ReadRawData (IndexInfo (), header)) {
			BYTE* ptr = header;
			ptr += 16 + sizeof (TreeTYPE) + sizeof (uint) + sizeof (uint);
			*(IndexInfo*)ptr = info;
			IOManager.SaveRawData (IndexInfo (), header);
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
	}

	//��infoָ��Ľڵ���뵽freelist��
	void dropNode (IndexInfo &info) {
		IOManager.erase (info);
	}

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
			BPlusNode<_KTy>* tmp = GetNodePtr (nullNode);
			tmp->Index[0].key = key;
			tmp->Index[0].info = data;
			setRootInfo (nullNode);
		}
		else {
			Root->Search (key)->insert (key, data);
		}
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
		if (Root->getSize () == 1) {
			setRootInfo (Root->Index[0].info);
			IOManager.erase (rootInfo);
		}
		else if (!Root->getSize ()) {
			setRootInfo (IndexInfo ());
			IOManager.erase (rootInfo);
		}
		return true;
	}
};

//��֤ÿ��nodeռ��һ��disk page������IO�ٶ�
class IndexManager {
private:
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


