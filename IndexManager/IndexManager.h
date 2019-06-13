#pragma once
#include <map>
#include "..\miniSQL\Commonheader.h"
#include "..\BPTree\BPTree_V3.h"

enum TreeTYPE {
	INT,
	FLOAT,
	STRING
};

//��֤ÿ��nodeռ��һ��disk page������IO�ٶ�
template<typename _KTy>
class IndexManager {

	IndexManager ();
	~IndexManager();

	//��ֵ����
	const IndexInfo find (_KTy key);
	//��Χ����
	const vector<IndexInfo> find (_KTy min, _KTy max);
	//�޸���������ط���
	//����,���ڽ�����index,��key�ظ�����������������ｫ���׳��쳣
	void insert (const _KTy &key, const IndexInfo & data);
	//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
	bool erase (const _KTy &key);
	//drop, ��root = nullptr ����
	void dropIndex ();
	//���������µ�buffer
	void update ();
	//��BYTE*������ת����node
	template<typename _KTy>BPlusNode<_KTy> &getNode (const BYTE (&rawData)[MAX_BLOCK_SIZE]);

	//1��IndexManagerʵ������һ�������ļ�
	string fileName;
	//Rootָ�룬��union��Լ�ռ�
	BPlusNode<_KTy> Root;
	map<IndexInfo, 
};

ostream& operator <<(ostream& os, const IndexInfo& info);