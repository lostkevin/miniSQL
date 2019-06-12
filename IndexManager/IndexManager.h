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
	//����,���ڽ�����index,��key�ظ�����������������ｫ���׳��쳣
	template<typename _KTy> void insert (const _KTy &key, const IndexInfo & data);
	//ɾ������key�����ڷ���false, ɾ���ɹ�����true,�������쳣
	template<typename _KTy> bool erase (const _KTy &key);
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

template<typename _KTy>
inline const IndexInfo IndexManager::find (_KTy key)
{
	return IndexInfo ();
}

template<typename _KTy>
inline const vector<IndexInfo> IndexManager::find (_KTy min, _KTy max)
{
	return vector<IndexInfo> ();
}

template<typename _KTy>
inline void IndexManager::insert (const _KTy & key, const IndexInfo & data)
{
}

template<typename _KTy>
inline bool IndexManager::erase (const _KTy & key)
{
	return false;
}
