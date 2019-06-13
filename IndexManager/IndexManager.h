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

//保证每个node占用一个disk page，增加IO速度
class IndexManager {
private:
	template<typename _KTy>
	class Index {
		Index ();
		~Index ();		
		//drop, 令root = nullptr 即可
		void dropIndex ();
		//将索引更新到buffer
		void update ();
		//将BYTE*的数据转换成node
		BPlusNode<_KTy> &getNode (const BYTE (&rawData)[MAX_BLOCK_SIZE]);
		//获取一个新Node
		BPlusNode<_KTy> &newNode ();
		//Root指针
		BPlusNode<_KTy> Root;
		map<IndexInfo, BPlusNode<_KTy>> nodesActive;
	};

public:
	//1个IndexManager实例管理一个索引文件
	string fileName;
	TreeTYPE type;
	bool setIndexInfo ();
	IndexManager (const string &fileName, BufferManager& bufferMgr);
	//等值搜索
	template<typename _Ty> const vector<IndexInfo> find (_Ty min, _Ty max);
	//范围搜索
	template<typename _Ty> const IndexInfo find (_Ty key);
	//修改索引的相关方法
	//插入,用于建立新index,若key重复将会更新索引，这里将会抛出异常
	template<typename _Ty> void insert (const _Ty &key, const IndexInfo & data);
	//删除，若key不存在返回false, 删除成功返回true,否则抛异常
	template<typename _Ty> bool erase (const _Ty &key);

	~IndexManager ();
};