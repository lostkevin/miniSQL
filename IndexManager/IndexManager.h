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
	//是否加载到内存中
	bool loadState;
	//块内偏移量
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
//未define时允许load\define
//define后允许drop\save\insert\erase\find
class IndexManager {
public:
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
	//drop, 销毁索引,undefine index
	void dropIndex ();
	//save, 保存索引至文件
	bool save (const string &fileName);
	//load，读取索引
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
