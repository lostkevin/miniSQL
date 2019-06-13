#pragma once
#include <string>
#include <vector>
#include "..\miniSQL\B+Tree_V2.h"
using namespace std;

class IndexInfo {
private:
	uint _blockID;
	string _fileName;
	
	//����ƫ����
	uint _blockOffset;
	//�ļ�ƫ����
	uint _fileOffset;
public:
	friend istream & operator>>(istream & is, IndexInfo & info);
	IndexInfo () {
		_fileOffset = _blockID = _blockOffset = 0;
		_loadState = false;
	}
	uint blockID () const {
		return _blockID;
	}
	const string& fileName () const {
		return _fileName;
	}
	uint blockOffset () const {
		return _blockOffset;
	}
	uint fileOffset () const {
		return _fileOffset;
	}
	//�Ƿ���ص��ڴ���
	bool _loadState;
};

enum TreeTYPE {
	INT,
	FLOAT,
	STRING
};

//Index Manager
//δdefineʱ����load\define
//define������drop\save\insert\erase\find

//������������ʱ�������ڴ���
//��֤ÿ��nodeռ��һ��disk page������IO�ٶ�
class IndexManager {
public:
	typedef BPlusTree<int, IndexInfo> IntTree;
	typedef BPlusTree<float, IndexInfo> FloatTree;
	typedef BPlusTree<string, IndexInfo> CharTree;
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
	//drop, ��������,undefine index,�����ļ����ⲿɾ��
	void dropIndex ();
	//save, �����������ļ�,��IndexManager undef return false
	bool save (const string &fileName);
	//load����ȡ����
	bool load (const string &fileName);
	
private:
	bool defined;
	void * BPTree;
	TreeTYPE treeType;

	uint getSize () {
		if (BPTree) {
			if (!defined)throw new exception ("Exist a tree without keytype defined!");
			switch (treeType) {
			case INT: return ((IntTree*)BPTree)->getSize ();
			case FLOAT: return ((FloatTree*)BPTree)->getSize ();
			case STRING: return ((CharTree*)BPTree)->getSize ();
			}
		}
		return 0;
	}
	uint getNodeSize ();
};

ostream& operator <<(ostream& os, const IndexInfo& info);