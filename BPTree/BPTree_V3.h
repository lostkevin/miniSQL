#pragma once
#define DEBUG
#ifndef uint
#define uint unsigned int
#endif
#include "..\miniSQL\Commonheader.h"
using namespace std;

enum NodeType {
	LEAF,
	NONLEAF
};

template<typename _KTy>
class Pair {
public:
	_KTy key;
	IndexInfo info;
};

//索引即数据
//每个节点皆有一个变量表示该节点是否为leaf
template<typename _KTy>
class BPlusNode {
public:
	typedef BPlusNode<_KTy> Node;
	typedef Node* NodePtr;
	typedef Pair<_KTy> Pair;
private:
	//获得指向某一个节点的指针，该节点由索引信息确定
	NodePtr (&GetNodePtr)(const IndexInfo&);
	//获得一个新node
	const IndexInfo (&GetNewNode)();
	bool IsDirty;
	uint _order;

	void updateKey (_KTy oldKey, _KTy newKey) {
		for (uint i = 0; i < this->size; i++)
			if (this->Index[i].key == oldKey) {
				this->IsDirty = true;
				this->Index[i].key = newKey;
				break;
			}
		if (GetNodePtr(this->Parent))GetNodePtr (this->Parent)->updateKey (oldKey, newKey);
	}

public:


	IndexInfo thisPos;
	NodeType type;
	Pair * Index;
	IndexInfo Parent;
	//左右兄弟的索引
	IndexInfo LIndex;
	IndexInfo RIndex;
	uint size;

	BPlusNode (const BPlusNode& r) {
		*this = r;
	}

	~BPlusNode () {
		delete Index;
	}

	//查找拥有键值key的某节点
	IndexInfo find (_KTy key) {
		if(this->type == NONLEAF) {
			uint i = 0;
			for (; i + 1 < this->size && key >= this->Index[i].key; i++);
			return GetNodePtr (this->Index[i].info)->find (key);
		}
		else {
			uint i = 0;
			for (; i < this->size && key != this->Index[i].key; i++);
			return i == this->size ? IndexInfo () : this->Index[i].info;
		}
	}

	//范围查询
	void find (_KTy min, _KTy max, vector<IndexInfo>& result) {
		NodePtr ptr = Search (min);
		uint i = 0;
		//leaf node
		for (; i + 1 < ptr->size && min > ptr->Index[i].key; i++);
		if (i == this->size)return;
		while (ptr) {
			if (ptr->Index[i].key <= max)result.push_back (ptr->Index[i].info);
			else break;
			if (++i == ptr->size) {
				i = 0;
				ptr = GetNodePtr (ptr->RIndex);
			}
		}
	}

	void insert (_KTy key, const IndexInfo & info) {
		//可以证明key>=this->key[0]
		this->IsDirty = true;
		uint i = 0;
		if (this->type == LEAF)
		{
			for (; i < this->size && key > this->Index[i].key; i++);
			if (i == this->size)
			{
				this->Index[i] = Pair (key, info);
				this->size++;
			}
			else if (key == this->Index[i].key) {
				this->Index[i].info = info;
			}
			else {
				for (int j = this->size; j > (int)i; j--)this->Index[j] = this->Index[j - 1];
				this->Index[i] = Pair (key, info);
				this->size++;
			}
			if (this->size == _order + 1)
			{
				IndexInfo newInfo = GetNewNode ();
				NodePtr newNode = GetNodePtr (newInfo);
				newNode->type = this->type;
				for (uint i = size / 2; i <= _order; i++)
				{
					newNode->Index[newNode->size] = this->Index[i];
					this->Index[i] = Pair();
					newNode->size++;
				}
				this->size /= 2;
				newNode->Parent = Parent;
				newNode->LIndex = thisPos;
				newNode->RIndex = this->RIndex;
				if (GetNodePtr(this->RIndex))
					GetNodePtr (this->RIndex)->LIndex = newInfo;
				this->RIndex = newInfo;
				if(GetNodePtr(Parent))return Parent->Insert (newNode->Index[0].key, newInfo);
			}
		}
		else
		{
			for (; i + 1 < this->size && key >= this->Index[i].key; i++);
			if (i + 1 == this->size)
			{
				this->Index[i].key = key;
				this->size++;
				this->Index[i + 1].info = info;
			}
			else
			{
				for (int j = this->size; j > (int)i; j--) this->Index[j] = this->Index[j - 1];
				this->Index[i].key = key;
				this->Index[i + 1].info = info;
				this->size++;
			}
			if (this->size == _order + 1)
			{
				IndexInfo newInfo = GetNewNode ();
				NodePtr newNode = GetNodePtr (newInfo);
				newNode->type = this->type;
				for (uint i = size / 2; i <= _order; i++)
				{
					newNode->Index[newNode->size] = this->Index[i];
					this->GetNodePtr (this->Index[i].info)->Parent = newInfo;
					this->Index[i] = Pair ();
					newNode->size++;
				}
				this->size /= 2;
				newNode->Parent = Parent;
				newNode->LIndex = thisPos;
				newNode->RIndex = this->RIndex;
				if (GetNodePtr (this->RIndex))
					GetNodePtr (this->RIndex)->LIndex = newInfo;
				this->RIndex = newInfo;
				if (GetNodePtr (Parent))return Parent->Insert (newNode->Index[0].key, newInfo);
			}
		}
	}

	//删除指定key的节点，如果key不存在，返回false，否则抛异常
	bool erase (_KTy key) {
		bool flag = false;
		for (uint i = 0; i < this->size; i++) {
			if (key == this->Index[i].key) {
				for (uint j = i + 1; j < this->size; j++) {
					this->Index[j - 1] = this->Index[j];
				}
				this->Index[--this->size] = Pair ();
				if (GetNodePtr(Parent))GetNodePtr (Parent)->updateKey (key, this->Index[i].key);
				flag = true;
				this->IsDirty = true;
				break;
			}
		}
		//3 4 5 6 7
		//1 1 2 2 3
		if (this->size == (_order - 1) / 2) {
			NodePtr LNode = GetNodePtr (LIndex);
			NodePtr RNode = GetNodePtr (RIndex);
			if (LNode && LNode->Parent == this->Parent && LNode->size > (_order - 1) / 2 + 1) {
				//从左兄弟取一个节点过来，左兄弟的size>=2，因此左兄弟不用更新key
				//但本节点需要再次update
				LNode->IsDirty = true;
				for (uint j = this->size; j > 0; j--) this->Index[j] = this->Index[j - 1];
				if (this->type == LEAF) {
					this->Index[0] = LNode->Index[LNode->size - 1];
					LNode->Index[LNode->size - 1] = Pair ();
					GetNodePtr(Parent)->updateKey (this->Index[1].key, this->Index[0].key);
				}
				else {
					this->Index[0] = LNode->Index[LNode->size - 1];
					GetNodePtr (LNode->Index[LNode->size - 1])->IsDirty = true;
					GetNodePtr (LNode->Index[LNode->size - 1])->Parent = thisPos;
					LNode->Index[LNode->size - 1] = Pair ();
					int i = 0;
					NodePtr parentPtr = GetNodePtr (this->Parent);
					for (; parentPtr->Index[i].info != this->LIndex; i++);
					this->Index[0].key = parentPtr->Index[i].key;
					parentPtr->updateKey (this->Index[0].key, LNode->Index[LNode->size - 2]);
				}
				LNode->size--;
				this->size++;
			}
			//左节点分不出，看看右节点有没有
			else if (RNode && RNode->Parent == this->Parent && RNode->size > (_order - 1) / 2 + 1) {
				//从右兄弟取一个节点过来，右兄弟需更新key
				//本节点不用更新
				RNode->IsDirty = true;
				if (this->Type == LEAF) {
					this->Index[this->size] = RNode->Index[0];
					RNode->Index[0] = Pair ();
					GetNodePtr (Parent)->updateKey (RNode->Index[0].key, RNode->Index[1].key);
				}
				else {
					this->Index[this->size] = RNode->Index[0];
					GetNodePtr (RNode->Index[0].info)->IsDirty = true;
					GetNodePtr (RNode->Index[0].info)->Parent = thisPos;
					int i = 0;
					NodePtr parentPtr = GetNodePtr (this->Parent);
					for (; parentPtr->Index[i].info != thisPos; i++);
					this->Index[this->size - 1].key = parentPtr->Index[i].key;
					parentPtr->updateKey (parentPtr->Index[i].key, RNode->Index[i].key);
				}
				for (uint j = 1; j < RNode->size; j++) {
					RNode->Index[j - 1] = RNode->Index[j];
				}
				RNode->Index[--RNode->size] = Pair();
				this->size++;
			}
			else {
				_KTy nextKey;
				//左右节点都没有,需要合并
				if (LNode && LNode->Parent == this->Parent) {
					uint i = 0;
					NodePtr parentPtr = GetNodePtr (this->Parent);
					for (; parentPtr->Index[i].info != this->LIndex; i++);
					nextKey = parentPtr->Index[i].key;
					//可以与左节点合并
					for (int i = this->size; i >= 0; i--) this->Index[i + LNode->size] = this->Index[i];
					for (uint i = 0; i < LNode->size; i++) {
						this->Index[i] = LNode->Index[i];
						if (this->type == NONLEAF) {
							GetNodePtr (LNode->Index[i].info)->IsDirty = true;
							GetNodePtr (LNode->Index[i].info)->Parent = thisPos;
						}
						LNode->Index[i] = Pair ();
					}
					if (this->type == NONLEAF)this->Index[LNode->size - 1].key = nextKey;
					this->size += LNode->size;
					//从链表中取出
					NodePtr LL = GetNodePtr (LNode->LIndex);
					if (LL) {
						LL->IsDirty = true;
						LL->RIndex = thisPos;
					}
					this->Index = LNode->LIndex;
				}
				else if (RNode && RNode->Parent == this->Parent) {
					uint i = 0;
					NodePtr parentPtr = GetNodePtr (this->Parent);
					for (; parentPtr->Index[i].info != this->LIndex; i++);
					nextKey = parentPtr->Index[i].key;
					//与右节点合并
					RNode->IsDirty = true;
					for (int i = RNode->size; i >= 0; i--) RNode->Index[i + this->size] = this->Index[i];
					for (uint i = 0; i < this->size; i++) {
						RNode->Index[i] = this->Index[i];
						if (this->type == NONLEAF) {
							GetNodePtr (this->Index[i].info)->IsDirty = true;
							GetNodePtr (this->Index[i].info)->Parent = RNode;
						}
						this->Index[i] = Pair ();
					}
					if (this->type == NONLEAF)RNode->Index[this->size - 1].key = nextKey;
					RNode->size += this->size;
					//从链表中取出
					RNode->LIndex = this->LIndex;
					if (LNode)LNode->RIndex = this->RIndex;
				}
				else {
					//都不可以合并，说明该节点的父亲节点若存在，仅有一个儿子，这种情况仅出现在该节点为根
					if (this->Parent)throw new std::exception ("This node is not a root!");
					//根的修改需要交给外部完成，若this->size!=0,外部不需要做任何操作,否则释放内存并初始化root=nullptr
					return true;
				}
				//合并结束，删除父节点中的被合并节点
				GetNodePtr (Parent)->erase (nextKey);
			}
		}
		return flag;
	}

	BPlusNode (uint order, NodePtr (&GetNodePtr)(const IndexInfo&) , const IndexInfo (&GetNewNode)() , bool IsDirty = false)
		: GetNodePtr(GetNodePtr), GetNewNode(GetNewNode){
		this->_order = order;
		this->IsDirty = IsDirty;
	}

	//深拷贝
	const BPlusNode & operator =(const BPlusNode& rValue) {
		if (this->_order != rValue._order)throw new exception ("order not equal");
		this->type = rValue.type;
		if (this->Index)delete this->Index;
		this->Index = new Pair[_order + 1];
		memcpy_s (this->Index, sizeof (Pair) * (_order + 1), rValue.Index, sizeof (Pair) * (_order + 1));
		this->LIndex = rValue.LIndex;
		this->RIndex = rValue.RIndex;
		this->Parent = rValue.Parent;
		this->GetNewNode = rValue.GetNewNode;
		this->GetNodePtr = rValue.GetNodePtr;
		this->IsDirty = rValue.IsDirty;
		this->size = rValue.size;
		return *this;
	}

	bool getDirtyState () {
		return IsDirty;
	}

	uint getOrder () { return _order; }

	NodePtr Search (_KTy key) {
		if (this->type == NONLEAF) {
			uint i = 0;
			for (; i + 1 < this->size && key >= this->Index[i].key; i++);
			return GetNodePtr (this->Index[i].info)->Search (key);
		}
		else return this;
	}

#ifdef DEBUG
	void debug();
#endif
};
