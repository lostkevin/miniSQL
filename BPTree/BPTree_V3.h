#pragma once
#define DEBUG
#include <vector>
using namespace std;

//索引即数据
//每个节点皆有一个变量表示该节点是否为leaf
template<typename _KTy>
class BPlusNode {
	uint order;
	//获得指向某一个节点的指针，该节点由索引信息确定
	BPlusNode* (*GetNodePtr)(const IndexInfo&); 
	//获得一个新node
	const IndexInfo (*GetNewNode)();
	//左右兄弟的索引
	IndexInfo LIndex;
	IndexInfo RIndex;
	bool IsDirty;
	uint _size;
	uint _order;
public:
	class Pair {
		_KTy key;
		IndexInfo info;
	};
	Pair * Index;
	IndexInfo Parent;
	//插入指定key的节点，如果插入失败，返回false，否则返回true
	void insert (_KTy key, const IndexInfo &);
	//删除指定key的节点，如果key不存在，返回false，否则抛异常
	bool erase (_KTy key);
	~BPlusNode ();
	//查找拥有键值key的某节点
	IndexInfo find (_KTy);
	uint getSize () { return _size };
	uint getOrder () { return _order; }
	//范围查询
	void find (_KTy min, _KTy max, vector<IndexInfo>& result);
	bool getDirtyState () {
		return IsDirty;
	}
	BPlusNode (uint order, BPlusNode* (&GetNodePtr)(const IndexInfo&) , const IndexInfo (&GetNewNode)() , bool IsDirty = false)
		: GetNodePtr(GetNodePtr), GetNewNode(GetNewNode){
		this->order = order;
		this->IsDirty = IsDirty;
	}
#ifdef DEBUG
	void debug();
#endif
};
