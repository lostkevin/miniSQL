#pragma once
#include <map>
#include "..\miniSQL\Commonheader.h"
#include "..\BPTree\BPTree_V3.h"

enum TreeTYPE {
	UNDEF,
	INT,
	FLOAT,
	STRING
};

//��֤ÿ��nodeռ��һ��disk page������IO�ٶ�
class IndexManager {
private:
	template<typename _KTy>
	class Index {
		Index ();
		~Index ();		
		//drop, ��root = nullptr ����
		void dropIndex ();
		//���������µ�buffer
		void update ();
		//��BYTE*������ת����node
		BPlusNode<_KTy> &getNode (const BYTE (&rawData)[MAX_BLOCK_SIZE]);
		//��ȡһ����Node
		BPlusNode<_KTy> &newNode ();
		//Rootָ��
		BPlusNode<_KTy> Root;
		map<IndexInfo, BPlusNode<_KTy>> nodesActive;
	};

public:
	//1��IndexManagerʵ������һ�������ļ�
	string fileName;
	TreeTYPE type;
	bool setIndexInfo ();
	IndexManager (const string &fileName, BufferManager& bufferMgr);
	//��ֵ����
	template<typename _Ty> const vector<IndexInfo> find (_Ty min, _Ty max);
	//��Χ����
	template<typename _Ty> const IndexInfo find (_Ty key);
	//�޸���������ط���
	//����,���ڽ�����index,��key�ظ�����������������ｫ���׳��쳣
	template<typename _Ty> void insert (const _Ty &key, const IndexInfo & data);
	//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
	template<typename _Ty> bool erase (const _Ty &key);

	~IndexManager ();
};