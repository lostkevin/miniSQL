#pragma once
#include <string>
#include "..\miniSQL\B+Tree_V2.h"
struct IndexInfo {
	uint blockID;
	uint offset;

	IndexInfo () {
		blockID = offset = 0;
	}
};

enum TreeTYPE {
	INT,
	FLOAT,
	STRING
};

//Index Manager
//δdefineʱ����load\define
//define������drop\save\insert\erase\find
class IndexManager {
public:
	IndexManager ();
	~IndexManager();

	//��ֵ����
	template<typename _KTy>IndexInfo find (_KTy key);
	//��Χ����
	template<typename _KTy>IndexInfo find (_KTy min, _KTy max);

	//�޸���������ط���
	//defineTree,�ڶ���ʵ��������ȷ������������Ϣ
	//��index��define����ʵ������֧��insert/eraseֱ����index��drop
	bool defineTree (TreeTYPE type);
	//����,���ڽ�����index,��key�ظ������������
	template<typename _KTy> void insert (_KTy key, IndexInfo data);
	//ɾ������key��������ʲôҲ����
	template<typename _KTy> void erase (_KTy key);
	//drop, ��������,undefine index
	void dropIndex ();
	//save, �����������ļ�
	bool save (string fileName);
	//load����ȡ����
	void load (string fileName);
private:
	bool defined;
	void * BPTree;
	TreeTYPE TreeType;
};

