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
//未define时允许load\define
//define后允许drop\save\insert\erase\find
class IndexManager {
public:
	IndexManager ();
	~IndexManager();

	//等值搜索
	template<typename _KTy>IndexInfo find (_KTy key);
	//范围搜索
	template<typename _KTy>IndexInfo find (_KTy min, _KTy max);

	//修改索引的相关方法
	//defineTree,在堆上实例化树，确定索引基本信息
	//若index被define，该实例将仅支持insert/erase直到该index被drop
	bool defineTree (TreeTYPE type);
	//插入,用于建立新index,若key重复将会更新索引
	template<typename _KTy> void insert (_KTy key, IndexInfo data);
	//删除，若key不存在则什么也不做
	template<typename _KTy> void erase (_KTy key);
	//drop, 销毁索引,undefine index
	void dropIndex ();
	//save, 保存索引至文件
	bool save (string fileName);
	//load，读取索引
	void load (string fileName);
private:
	bool defined;
	void * BPTree;
	TreeTYPE TreeType;
};

