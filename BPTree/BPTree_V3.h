#pragma once
#ifndef uint
#define uint unsigned int
#endif // !uint
#define DEBUG

#include <vector>
#include <string>
using namespace std;
enum NodeType { LEAF, NONLEAF };

class IndexInfo {

};


//索引即数据
//每个节点皆有一个变量表示该节点是否为leaf
template<typename _KTy>
class BPluseNode {
	uint order;
	IndexInfo LIndex;
	IndexInfo RIndex;
	//获得指向某一个节点的指针，该节点由索引信息确定
	BPluseNode* (*GetNodePtr)(const IndexInfo&); 
	//获得一个新node
	const IndexInfo (*GetNewNode)();
	//子索引
	Pair *index;
	//左右兄弟的索引
public:
	class Pair {
		_KTy key;
		IndexInfo info;
	};
	//插入指定key的节点，如果插入失败，返回false，否则返回true
	bool insert (_KTy key, const IndexInfo &);
	//删除指定key的节点，如果key不存在，返回false，否则抛异常
	bool erase (_KTy key);
	~BPluseNode ();
	//查找拥有键值key的某节点
	IndexInfo find (_KTy);
	//范围查询
	vector<IndexInfo> find (_KTy min, _KTy max);
	BPluseNode (uint order, BPluseNode* (*)(const IndexInfo&) GetNodePtr, const IndexInfo (*)() GetNewNode);
#ifdef DEBUG
	void debug();
#endif
};
