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
	//子索引
	Pair *index;
	//左右兄弟的索引
	IndexInfo LIndex;
	IndexInfo RIndex;
public:
	class Pair {
		_KTy key;
		IndexInfo info;
	};
	//插入指定key的节点，如果插入失败，返回false，否则返回true
	bool insert (_KTy key, const IndexInfo &);
	//删除指定key的节点，如果key不存在，返回false，否则抛异常
	bool erase (_KTy key);
	~BPlusNode ();
	//查找拥有键值key的某节点
	IndexInfo find (_KTy);
	//范围查询
	vector<IndexInfo> find (_KTy min, _KTy max);
	BPlusNode (uint order, BPlusNode* (*)(const IndexInfo&) GetNodePtr, const IndexInfo (*)() GetNewNode);
#ifdef DEBUG
	void debug();
#endif
};
