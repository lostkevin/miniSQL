#pragma once
#include <string>
#include <vector>
#include "..\miniSQL\B+Tree_V2.h"
using namespace std;

class IndexInfo {
private:
	uint _blockID;
	string _fileName;
	
	//块内偏移量
	uint _blockOffset;
	//文件偏移量
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
	//是否加载到内存中
	bool _loadState;
};

enum TreeTYPE {
	INT,
	FLOAT,
	STRING
};

//Index Manager
//未define时允许load\define
//define后允许drop\save\insert\erase\find

//所有索引运行时建立在内存中
//保证每个node占用一个disk page，增加IO速度
class IndexManager {
public:
	typedef BPlusTree<int, IndexInfo> IntTree;
	typedef BPlusTree<float, IndexInfo> FloatTree;
	typedef BPlusTree<string, IndexInfo> CharTree;
	IndexManager ();
	~IndexManager();

	//等值搜索
	template<typename _KTy>const IndexInfo find (_KTy key);
	//范围搜索
	template<typename _KTy>const vector<IndexInfo> find (_KTy min, _KTy max);

	//修改索引的相关方法
	//defineTree,在堆上实例化树，确定索引基本信息
	//若index被define，该实例将仅支持insert/erase直到该index被drop
	bool defineTree (TreeTYPE type);
	//插入,用于建立新index,若key重复将会更新索引，这里将会抛出异常
	template<typename _KTy> void insert (const _KTy &key, const IndexInfo & data);
	//删除，若key不存在返回false, 删除成功返回true,否则抛异常
	template<typename _KTy> bool erase (const _KTy &key);
	//drop, 销毁索引,undefine index,索引文件由外部删除
	void dropIndex ();
	//save, 保存索引至文件,若IndexManager undef return false
	bool save (const string &fileName);
	//load，读取索引
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