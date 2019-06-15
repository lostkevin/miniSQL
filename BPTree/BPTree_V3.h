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

//����������
//ÿ���ڵ����һ��������ʾ�ýڵ��Ƿ�Ϊleaf
template<typename _KTy>
class BPlusNode {
public:
	typedef BPlusNode<_KTy> Node;
	typedef Node* NodePtr;
	typedef Pair<_KTy> Pair;
private:
	//���ָ��ĳһ���ڵ��ָ�룬�ýڵ���������Ϣȷ��
	NodePtr (&GetNodePtr)(const IndexInfo&);
	//���һ����node
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
	//�����ֵܵ�����
	IndexInfo LIndex;
	IndexInfo RIndex;
	uint size;

	BPlusNode (const BPlusNode& r) {
		*this = r;
	}

	~BPlusNode () {
		delete Index;
	}

	//����ӵ�м�ֵkey��ĳ�ڵ�
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

	//��Χ��ѯ
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
		//����֤��key>=this->key[0]
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

	//ɾ��ָ��key�Ľڵ㣬���key�����ڣ�����false���������쳣
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
				//�����ֵ�ȡһ���ڵ���������ֵܵ�size>=2��������ֵܲ��ø���key
				//�����ڵ���Ҫ�ٴ�update
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
			//��ڵ�ֲ����������ҽڵ���û��
			else if (RNode && RNode->Parent == this->Parent && RNode->size > (_order - 1) / 2 + 1) {
				//�����ֵ�ȡһ���ڵ���������ֵ������key
				//���ڵ㲻�ø���
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
				//���ҽڵ㶼û��,��Ҫ�ϲ�
				if (LNode && LNode->Parent == this->Parent) {
					uint i = 0;
					NodePtr parentPtr = GetNodePtr (this->Parent);
					for (; parentPtr->Index[i].info != this->LIndex; i++);
					nextKey = parentPtr->Index[i].key;
					//��������ڵ�ϲ�
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
					//��������ȡ��
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
					//���ҽڵ�ϲ�
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
					//��������ȡ��
					RNode->LIndex = this->LIndex;
					if (LNode)LNode->RIndex = this->RIndex;
				}
				else {
					//�������Ժϲ���˵���ýڵ�ĸ��׽ڵ������ڣ�����һ�����ӣ���������������ڸýڵ�Ϊ��
					if (this->Parent)throw new std::exception ("This node is not a root!");
					//�����޸���Ҫ�����ⲿ��ɣ���this->size!=0,�ⲿ����Ҫ���κβ���,�����ͷ��ڴ沢��ʼ��root=nullptr
					return true;
				}
				//�ϲ�������ɾ�����ڵ��еı��ϲ��ڵ�
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

	//���
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
