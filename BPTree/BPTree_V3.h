#pragma once
#include <functional>
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
template<typename _KTy> class Index;

//ÿ���ڵ����һ��������ʾ�ýڵ��Ƿ�Ϊleaf
template<typename _KTy>
class BPlusNode {
public:
	typedef BPlusNode<_KTy> Node;
	typedef Node* NodePtr;
	typedef Pair<_KTy> Pair;
private:
	bool IsDirty;
	uint _order;

	void updateKey (_KTy oldKey, _KTy newKey) {
		for (uint i = 0; i < this->size; i++)
			if (this->index[i].key == oldKey) {
				this->IsDirty = true;
				this->index[i].key = newKey;
				break;
			}
		if (outer->GetNodePtr(this->Parent))outer->GetNodePtr (this->Parent)->updateKey (oldKey, newKey);
	}

public:
	Index<_KTy>* outer;
	IndexInfo thisPos;
	NodeType type;
	Pair * index;
	IndexInfo Parent;
	//�����ֵܵ�����
	IndexInfo LIndex;
	IndexInfo RIndex;
	uint size;

	BPlusNode (const BPlusNode& r) {
		*this = r;
	}

	~BPlusNode () {
		delete index;
	}

	//����ӵ�м�ֵkey��ĳ�ڵ�
	IndexInfo find (_KTy key) {
		if(this->type == NONLEAF) {
			uint i = 0;
			for (; i + 1 < this->size && key >= this->index[i].key; i++);
			return outer->GetNodePtr (this->index[i].info)->find (key);
		}
		else {
			uint i = 0;
			for (; i < this->size && key != this->index[i].key; i++);
			return i == this->size ? IndexInfo () : this->index[i].info;
		}
	}

	//��Χ��ѯ
	void find (_KTy min, _KTy max, vector<IndexInfo>& result) {
		NodePtr ptr = Search (min);
		uint i = 0;
		//leaf node
		for (; i + 1 < ptr->size && min > ptr->index[i].key; i++);
		if (i == this->size)return;
		while (ptr) {
			if (ptr->index[i].key <= max)result.push_back (ptr->index[i].info);
			else break;
			if (++i == ptr->size) {
				i = 0;
				ptr = outer->GetNodePtr (ptr->RIndex);
			}
		}
	}

	void insert (_KTy key, const IndexInfo & info) {
		//����֤��key>=this->key[0]
		this->IsDirty = true;
		uint i = 0;
		if (this->type == LEAF)
		{
			for (; i < this->size && key > this->index[i].key; i++);
			if (i == this->size)
			{
				this->index[i] = Pair{ key, info };
				this->size++;
			}
			else if (key == this->index[i].key) {
				this->index[i].info = info;
			}
			else {
				for (int j = this->size; j > (int)i; j--)this->index[j] = this->index[j - 1];
				this->index[i] = Pair{ key, info };
				this->size++;
			}
			if (this->size == _order + 1)
			{
				IndexInfo newInfo = outer->GetNewNode ();
				NodePtr newNode = outer->GetNodePtr (newInfo);
				newNode->type = this->type;
				for (uint i = size / 2; i <= _order; i++)
				{
					newNode->index[newNode->size] = this->index[i];
					this->index[i] = Pair();
					newNode->size++;
				}
				this->size /= 2;
				newNode->Parent = Parent;
				newNode->LIndex = thisPos;
				newNode->RIndex = this->RIndex;
				if (outer->GetNodePtr(this->RIndex))
					outer->GetNodePtr (this->RIndex)->LIndex = newInfo;
				this->RIndex = newInfo;
				if(outer->GetNodePtr(Parent))return outer->GetNodePtr (Parent)->insert (newNode->index[0].key, newInfo);
			}
		}
		else
		{
			for (; i + 1 < this->size && key >= this->index[i].key; i++);
			if (i + 1 == this->size)
			{
				this->index[i].key = key;
				this->size++;
				this->index[i + 1].info = info;
			}
			else
			{
				for (int j = this->size; j > (int)i; j--) this->index[j] = this->index[j - 1];
				this->index[i].key = key;
				this->index[i + 1].info = info;
				this->size++;
			}
			if (this->size == _order + 1)
			{
				IndexInfo newInfo = outer->GetNewNode ();
				NodePtr newNode = outer->GetNodePtr (newInfo);
				newNode->type = this->type;
				for (uint i = size / 2; i <= _order; i++)
				{
					newNode->index[newNode->size] = this->index[i];
					this->outer->GetNodePtr (this->index[i].info)->Parent = newInfo;
					this->index[i] = Pair ();
					newNode->size++;
				}
				this->size /= 2;
				newNode->Parent = Parent;
				newNode->LIndex = thisPos;
				newNode->RIndex = this->RIndex;
				if (outer->GetNodePtr (this->RIndex))
					outer->GetNodePtr (this->RIndex)->LIndex = newInfo;
				this->RIndex = newInfo;
				if (outer->GetNodePtr (Parent))return outer->GetNodePtr (Parent)->insert (newNode->index[0].key, newInfo);
			}
		}
	}

	//ɾ��ָ��key�Ľڵ㣬���key�����ڣ�����false���������쳣
	bool erase (_KTy key) {
		bool flag = false;
		for (uint i = 0; i < this->size; i++) {
			if (key == this->index[i].key) {
				for (uint j = i + 1; j < this->size; j++) {
					this->index[j - 1] = this->index[j];
				}
				this->index[--this->size] = Pair ();
				if (outer->GetNodePtr(Parent))outer->GetNodePtr (Parent)->updateKey (key, this->index[i].key);
				flag = true;
				this->IsDirty = true;
				break;
			}
		}
		//3 4 5 6 7
		//1 1 2 2 3
		if (this->size == (_order - 1) / 2) {
			NodePtr LNode = outer->GetNodePtr (LIndex);
			NodePtr RNode = outer->GetNodePtr (RIndex);
			if (LNode && LNode->Parent == this->Parent && LNode->size > (_order - 1) / 2 + 1) {
				//�����ֵ�ȡһ���ڵ���������ֵܵ�size>=2��������ֵܲ��ø���key
				//�����ڵ���Ҫ�ٴ�update
				LNode->IsDirty = true;
				for (uint j = this->size; j > 0; j--) this->index[j] = this->index[j - 1];
				if (this->type == LEAF) {
					this->index[0] = LNode->index[LNode->size - 1];
					LNode->index[LNode->size - 1] = Pair ();
					outer->GetNodePtr(Parent)->updateKey (this->index[1].key, this->index[0].key);
				}
				else {
					this->index[0] = LNode->index[LNode->size - 1];
					outer->GetNodePtr (LNode->index[LNode->size - 1].info)->IsDirty = true;
					outer->GetNodePtr (LNode->index[LNode->size - 1].info)->Parent = thisPos;
					LNode->index[LNode->size - 1] = Pair ();
					int i = 0;
					NodePtr parentPtr = outer->GetNodePtr (this->Parent);
					for (; parentPtr->index[i].info != this->LIndex; i++);
					this->index[0].key = parentPtr->index[i].key;
					parentPtr->updateKey (this->index[0].key, LNode->index[LNode->size - 2].key);
				}
				LNode->size--;
				this->size++;
			}
			//��ڵ�ֲ����������ҽڵ���û��
			else if (RNode && RNode->Parent == this->Parent && RNode->size > (_order - 1) / 2 + 1) {
				//�����ֵ�ȡһ���ڵ���������ֵ������key
				//���ڵ㲻�ø���
				RNode->IsDirty = true;
				if (this->type == LEAF) {
					this->index[this->size] = RNode->index[0];
					RNode->index[0] = Pair ();
					outer->GetNodePtr (Parent)->updateKey (RNode->index[0].key, RNode->index[1].key);
				}
				else {
					this->index[this->size] = RNode->index[0];
					outer->GetNodePtr (RNode->index[0].info)->IsDirty = true;
					outer->GetNodePtr (RNode->index[0].info)->Parent = thisPos;
					int i = 0;
					NodePtr parentPtr = outer->GetNodePtr (this->Parent);
					for (; parentPtr->index[i].info != thisPos; i++);
					this->index[this->size - 1].key = parentPtr->index[i].key;
					parentPtr->updateKey (parentPtr->index[i].key, RNode->index[i].key);
				}
				for (uint j = 1; j < RNode->size; j++) {
					RNode->index[j - 1] = RNode->index[j];
				}
				RNode->index[--RNode->size] = Pair();
				this->size++;
			}
			else {
				_KTy nextKey;
				//���ҽڵ㶼û��,��Ҫ�ϲ�
				if (LNode && LNode->Parent == this->Parent) {
					uint i = 0;
					NodePtr parentPtr = outer->GetNodePtr (this->Parent);
					for (; parentPtr->index[i].info != this->LIndex; i++);
					nextKey = parentPtr->index[i].key;
					//��������ڵ�ϲ�
					for (int i = this->size; i >= 0; i--) this->index[i + LNode->size] = this->index[i];
					for (uint i = 0; i < LNode->size; i++) {
						this->index[i] = LNode->index[i];
						if (this->type == NONLEAF) {
							outer->GetNodePtr (LNode->index[i].info)->IsDirty = true;
							outer->GetNodePtr (LNode->index[i].info)->Parent = thisPos;
						}
						LNode->index[i] = Pair ();
					}
					if (this->type == NONLEAF)this->index[LNode->size - 1].key = nextKey;
					this->size += LNode->size;
					//��������ȡ��
					NodePtr LL = outer->GetNodePtr (LNode->LIndex);
					if (LL) {
						LL->IsDirty = true;
						LL->RIndex = thisPos;
					}
					this->LIndex = LNode->LIndex;
				}
				else if (RNode && RNode->Parent == this->Parent) {
					uint i = 0;
					NodePtr parentPtr = outer->GetNodePtr (this->Parent);
					for (; parentPtr->index[i].info != this->LIndex; i++);
					nextKey = parentPtr->index[i].key;
					//���ҽڵ�ϲ�
					RNode->IsDirty = true;
					for (int i = RNode->size; i >= 0; i--) RNode->index[i + this->size] = this->index[i];
					for (uint i = 0; i < this->size; i++) {
						RNode->index[i] = this->index[i];
						if (this->type == NONLEAF) {
							(outer->GetNodePtr (this->index[i].info))->IsDirty = true;
							(outer->GetNodePtr (this->index[i].info))->Parent = RNode->thisPos;
						}
						this->index[i] = Pair ();
					}
					if (this->type == NONLEAF)RNode->index[this->size - 1].key = nextKey;
					RNode->size += this->size;
					//��������ȡ��
					RNode->LIndex = this->LIndex;
					if (LNode)LNode->RIndex = this->RIndex;
				}
				else {
					//�������Ժϲ���˵���ýڵ�ĸ��׽ڵ������ڣ�����һ�����ӣ���������������ڸýڵ�Ϊ��
					if (outer->GetNodePtr(this->Parent))throw new std::exception ("This node is not a root!");
					//�����޸���Ҫ�����ⲿ��ɣ���this->size!=0,�ⲿ����Ҫ���κβ���,�����ͷ��ڴ沢��ʼ��root=nullptr
					return true;
				}
				//�ϲ�������ɾ�����ڵ��еı��ϲ��ڵ�
				outer->GetNodePtr (Parent)->erase (nextKey);
			}
		}
		return flag;
	}

	BPlusNode (uint order, Index<_KTy>* outer, bool IsDirty = false)
	{
		this->outer = outer;
		this->_order = order;
		this->IsDirty = IsDirty;
		this->index = new Pair[order + 1]{ };
	}

	BPlusNode ()
	{
		this->outer = nullptr;
		this->_order = 0;
		this->IsDirty = false;
	}

	//���
	const BPlusNode & operator =(const BPlusNode& rValue) {
		this->_order = rValue._order;
		this->type = rValue.type;
		this->index = new Pair[_order + 1];
		memcpy_s (this->index, sizeof (Pair) * (_order + 1), rValue.index, sizeof (Pair) * (_order + 1));
		this->LIndex = rValue.LIndex;
		this->RIndex = rValue.RIndex;
		this->Parent = rValue.Parent;
		this->outer = rValue.outer;
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
			for (; i + 1 < this->size && key >= this->index[i].key; i++);
			return outer->GetNodePtr (this->index[i].info)->Search (key);
		}
		else return this;
	}
};
