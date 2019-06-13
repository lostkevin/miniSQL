#pragma once
#include <map>
#include "..\miniSQL\Commonheader.h"
#include "..\BPTree\BPTree_V3.h"

enum TreeTYPE {
	INT,
	FLOAT,
	STRING
};

//保证每个node占用一个disk page，增加IO速度
template<typename _KTy>
class IndexManager {

	IndexManager ();
	~IndexManager();

	//等值搜索
	const IndexInfo find (_KTy key);
	//范围搜索
	const vector<IndexInfo> find (_KTy min, _KTy max);
	//修改索引的相关方法
	//插入,用于建立新index,若key重复将会更新索引，这里将会抛出异常
	void insert (const _KTy &key, const IndexInfo & data);
	//删除，若key不存在返回false, 删除成功返回true,否则抛异常
	bool erase (const _KTy &key);
	//drop, 令root = nullptr 即可
	void dropIndex ();
	//将索引更新到buffer
	void update ();
	//将BYTE*的数据转换成node
	template<typename _KTy>BPlusNode<_KTy> &getNode (const BYTE (&rawData)[MAX_BLOCK_SIZE]);

	//1个IndexManager实例管理一个索引文件
	string fileName;
	//Root指针，用union节约空间
	BPlusNode<_KTy> Root;
	map<IndexInfo, 
};

ostream& operator <<(ostream& os, const IndexInfo& info);