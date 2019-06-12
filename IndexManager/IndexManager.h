#pragma once
#include <string>
#include <vector>
#include "..\miniSQL\B+Tree_V2.h"
using namespace std;

struct IndexInfo {
	union {
		uint blockID;
		string fileName;
	};
	//�Ƿ���ص��ڴ���
	bool loadState;
	//����ƫ����
	uint offset;

	IndexInfo () {
		blockID = offset = 0;
		loadState = false;
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
	template<typename _KTy>const IndexInfo find (_KTy key);
	//��Χ����
	template<typename _KTy>const vector<IndexInfo> find (_KTy min, _KTy max);

	//�޸���������ط���
	//defineTree,�ڶ���ʵ��������ȷ������������Ϣ
	//��index��define����ʵ������֧��insert/eraseֱ����index��drop
	bool defineTree (TreeTYPE type);
	//����,���ڽ�����index,��key�ظ������������
	template<typename _KTy> void insert (const _KTy &key, const IndexInfo & data);
	//ɾ������key��������ʲôҲ����
	template<typename _KTy> void erase (const _KTy &key);
	//drop, ��������,undefine index
	void dropIndex ();
	//save, �����������ļ�
	bool save (const string &fileName);
	//load����ȡ����
	void load (const string &fileName);
private:
	bool defined;
	void * BPTree;
	TreeTYPE TreeType;
};

