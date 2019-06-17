#pragma once
#pragma once
#ifndef uint
#define uint unsigned int
#endif // !uint
#define DEBUG

#include <vector>
#include <string>
using namespace std;
enum NodeType { LEAF, NONLEAF };
template<typename _KTy, typename _DTy>class BPlusNode;




//需要排序，重载operator <
template<typename _KTy, typename _DTy>
class BPlusTree {
	typedef BPlusNode<_KTy, _DTy> Node;
	typedef Node* NodePtr;
	typedef _DTy* DataPtr;
	NodePtr root;

public:
	struct Pair {
		_KTy first;
		_DTy second;
	};
	BPlusTree (uint order) {
		size = 0;
		this->order = order;
		root = nullptr;
	}
	~BPlusTree () {
		delete root;
	}
	//插入指定key的节点，如果插入失败，返回false，否则返回true
	bool insert (_KTy key, const _DTy &Data) {
		DataPtr pData = new _DTy (Data);
		NodePtr InsertedNode = Search (key);
		if (!root) {
			InsertedNode = root = new Node (order);
		}
		if (InsertedNode->Insert (key, pData)) {
			if (root->Parent) {
				root = root->Parent; //至多上升一层
			}
			size++;
			return true;
		}
		return false;
	}
	//删除指定key的节点，如果key不存在，返回false，否则抛异常
	bool erase (_KTy key) {
		NodePtr ErasedNode = Search (key);
		if (!root)return false;

		bool tmp = ErasedNode->erase (key);
		if (!tmp)return false;
		size--;
		if (root->size == 1 && root->Type == NONLEAF) {
			NodePtr tmp = root;
			root = root->GetNodePointer (0);
			root->Parent = nullptr;
			tmp->ptr[0] = nullptr;
			delete tmp;
		}
		else if (!root->size) {
			delete root;
			root = nullptr;
		}
		return true;
	}
	//find all elements with key in [min, max]
	vector<DataPtr> find (_KTy min, _KTy max) {
		NodePtr ptr = Search (min);
		vector<DataPtr> res;
		if (!ptr)return res;
		uint i = 0;
		//leaf node
		for (;i < ptr->size && ptr->key[i] < min; i++);
		if (i == ptr->size)return res;
		while (ptr) {
			if (ptr->key[i] <= max)res.push_back (ptr->ptr[i]);
			else break;
			if (++i == ptr->size) {
				i = 0;
				ptr = ptr->RLeaf;
			}
		}
		return res;
	}
	DataPtr find (_KTy key) {
		NodePtr ptr = root;
		if (!root)return nullptr;
		while (ptr->Type == NONLEAF) {
			uint i = 0;
			for (; i + 1 < ptr->size && key >= ptr->key[i]; i++);
			ptr = ptr->GetNodePointer (i);
		}
		uint i = 0;
		for (; i < ptr->size && key != ptr->key[i]; i++);
		return i == ptr->size ? nullptr : ptr->GetDataPointer (i);
	}
	bool empty () {
		return this->root == nullptr;
	}
	uint getSize () {
		return this->size;
	}
	void getAllPair (vector<Pair> & v) {
		NodePtr ptr = root;
		if (!root)return;
		while (ptr->Type == NONLEAF) {
			ptr = ptr->GetNodePointer (0);
		}
		while (ptr) {
			for (uint i = 0; i < ptr->size; i++) {
				v.push_back (Pair{ ptr->key[0], *ptr->GetDataPointer (i) });
			}
			ptr = ptr->RLeaf;
		}
		return;
	}

#ifdef DEBUG
	void printData () {
		NodePtr ptr = root;
		if (!root)return;
		while (ptr->Type == NONLEAF) {
			ptr->printData ();
#ifdef PRINT
			std::cout << std::endl << "=====next level======" << std::endl;
#endif // PRINT

			ptr = ptr->GetNodePointer (0);
		}
		ptr->printData ();
	}
#endif
private:
	NodePtr Search (_KTy key) {
		if (!root)return nullptr;
		NodePtr ptr = root;
		while (ptr->Type == NONLEAF) {
			uint i = 0;
			for (; i + 1 < ptr->size && key >= ptr->key[i]; i++);
			ptr = ptr->GetNodePointer(i);
		}
		return ptr;
	}
	uint order;
	uint size;
};


//示意图
//NONLEAF
//Key1 < Key2 < Key3 < ... < KeyM - 1  maxPtr
//	< Key1 < Key2 < Key3 ... < KeyM - 1 >= KeyM - 1
//	LEAF
//	Key1 < Key2 ... < KeyM
//	= Key1 = Key2 ... = KeyM

template<typename _KTy, typename _DTy>
class BPlusNode {
private:
	uint _order;
public:
	void** ptr;
	_KTy* key;
	typedef _DTy* DataPtr;
	typedef BPlusNode* NodePtr;
	NodeType Type;
	NodePtr Parent;
	//这两个指针是为了delete
	NodePtr RLeaf;
	NodePtr LLeaf;
	uint size;

	BPlusNode (uint order) {
		this->_order = order;
		ptr = new void*[_order + 1];
		key = new _KTy[_order + 1];
		Type = LEAF;
		Parent = nullptr;
		size = 0; //指针的size
		RLeaf = LLeaf = nullptr;
		for (uint i = 0; i <= _order; i++) {
			ptr[i] = nullptr; key[i] = _KTy ();
		}

	}
	~BPlusNode () {
		if (this->Type != LEAF) {
			for (uint i = 0; i < size; i++)delete (NodePtr)(ptr[i]);
		}
		else {
			//析构，释放data
			for (uint i = 0; i < size; i++)delete (DataPtr)(ptr[i]);
		}
		delete ptr;
		delete key;
	}
	DataPtr GetDataPointer (uint index) {
		return (DataPtr)(index < size ? ptr[index] : nullptr);
	}
	NodePtr GetNodePointer (uint index) {
		return (NodePtr)(index < size ? ptr[index] : nullptr);
	}

	bool Insert (_KTy key, void* pData) {
		//可以证明key>=this->key[0]
		uint i = 0;
		if (this->Type == LEAF)
		{
			for (; i < this->size && key > this->key[i]; i++)
				;
			if (i == this->size)
			{
				this->key[i] = key;
				this->ptr[i] = pData;
				this->size++;
			}
			else if (key == this->key[i])
				return false;
			else
			{
				for (int j = this->size; j > (int)i; j--)
				{
					this->key[j] = this->key[j - 1];
					this->ptr[j] = this->ptr[j - 1];
				}
				this->key[i] = key;
				this->ptr[i] = pData;
				this->size++;
			}
			if (this->size == _order + 1)
			{
				NodePtr newNode = new BPlusNode(_order);
				newNode->Type = this->Type;
				for (uint i = size / 2; i <= _order; i++)
				{
					newNode->ptr[newNode->size] = this->ptr[i];
					this->ptr[i] = nullptr;
					newNode->key[newNode->size] = this->key[i];
					this->key[i] = _KTy();
					newNode->size++;
				}
				this->size /= 2;
				newNode->Parent = Parent;
				newNode->LLeaf = this;
				newNode->RLeaf = RLeaf;
				if (RLeaf)
					RLeaf->LLeaf = newNode;
				RLeaf = newNode;
				if (!Parent)
				{
					newNode->Parent = Parent = new BPlusNode(_order);
					Parent->Type = NONLEAF;
					Parent->key[0] = newNode->key[0];
					Parent->ptr[1] = newNode;
					Parent->ptr[0] = this;
					Parent->size = 2;
					return true;
				}
				return Parent->Insert(newNode->key[0], newNode);
			}
			return true;
		}
		else
		{
			for (; i + 1 < this->size && key >= this->key[i]; i++)
				;
			if (i + 1 == this->size)
			{
				this->key[i] = key;
				this->size++;
				this->ptr[i + 1] = pData;
			}
			else
			{
				for (int j = this->size; j > (int)i; j--)
				{
					this->ptr[j] = this->ptr[j - 1];
					this->key[j] = this->key[j - 1];
				}
				this->key[i] = key;
				this->ptr[i + 1] = pData;
				this->size++;
			}
			if (this->size == _order + 1)
			{
				NodePtr newNode = new BPlusNode(_order);
				newNode->Type = this->Type;
				for (uint i = size / 2; i <= _order; i++)
				{
					newNode->ptr[newNode->size] = this->ptr[i];
					this->GetNodePointer(i)->Parent = newNode;
					this->ptr[i] = nullptr;
					newNode->key[newNode->size] = this->key[i];
					this->key[i] = _KTy();
					newNode->size++;
				}
				this->size /= 2;
				newNode->Parent = Parent;
				newNode->LLeaf = this;
				newNode->RLeaf = RLeaf;
				if (RLeaf)
					RLeaf->LLeaf = newNode;
				RLeaf = newNode;
				if (!Parent)
				{
					newNode->Parent = Parent = new BPlusNode(_order);
					Parent->Type = NONLEAF;
					Parent->key[0] = this->key[this->size - 1];
					Parent->ptr[1] = newNode;
					Parent->ptr[0] = this;
					Parent->size = 2;
					return true;
				}
				return Parent->Insert(this->key[this->size - 1], newNode);
			}
			return true;
		}
	}
#ifdef DEBUG
	void printData () {
		uint size = this->size;
		if (this->Type == LEAF) {
			for (uint i = this->size; i < _order; i++) {
				if (this->ptr[i])throw new exception ();
			}
		}
		else {
			for (uint i = this->size + 1; i < _order; i++) {
				if (this->ptr[i])throw new exception ();
			}
		}

#ifdef PRINT
		if (this->Type == NONLEAF)size--;
		for (uint i = 0; i < size; i++)std::cout << this->key[i] << " ";
		if (RLeaf) {
			std::cout << " | ";
			RLeaf->printData ();
		}
#endif // PRINT

	}
#endif // DEBUG

	void updateKey (_KTy oldKey, _KTy newKey) {
		for (uint i = 0; i < this->size; i++)
			if (this->key[i] == oldKey) {
				this->key[i] = newKey;
				break;
			}
		if (this->Parent)this->Parent->updateKey (oldKey, newKey);
	}

	bool erase (_KTy key) {
		bool flag = false;
		for (uint i = 0; i < this->size; i++)
		{
			if (this->key[i] == key)
			{
				if (this->Type == LEAF)
				{
					delete (DataPtr)(this->ptr[i]);
				}
				else
				{
					delete (NodePtr)(this->ptr[i]);
				}
				this->ptr[i] = nullptr;
				for (uint j = i + 1; j < this->size; j++)
				{
					this->key[j - 1] = this->key[j];
					this->ptr[j - 1] = this->ptr[j];
				}
				this->ptr[--this->size] = nullptr;
				if (this->Parent)
					this->Parent->updateKey(key, this->key[i]);
				flag = true;
				break;
			}
		}
		//3 4 5 6 7
		//1 1 2 2 3
		if (this->size == (_order - 1) / 2)
		{
			if (LLeaf && LLeaf->Parent == this->Parent && LLeaf->size > (_order - 1) / 2 + 1)
			{
				//从左兄弟取一个节点过来，左兄弟的size>=2，因此左兄弟不用更新key
				//但本节点需要再次update
				for (uint j = this->size; j > 0; j--)
				{
					this->key[j] = this->key[j - 1];
					this->ptr[j] = this->ptr[j - 1];
				}
				this->ptr[0] = nullptr;
				if (this->Type == LEAF)
				{
					this->key[0] = LLeaf->key[LLeaf->size - 1];
					this->ptr[0] = LLeaf->ptr[LLeaf->size - 1];
					LLeaf->ptr[LLeaf->size - 1] = nullptr;
					this->Parent->updateKey(this->key[1], this->key[0]);
				}
				else
				{
					this->ptr[0] = LLeaf->ptr[LLeaf->size - 1];
					LLeaf->GetNodePointer(LLeaf->size - 1)->Parent = this;
					LLeaf->ptr[LLeaf->size - 1] = nullptr;
					int i = 0;
					for (; this->Parent->ptr[i] != this->LLeaf; i++)
						;
					this->key[0] = this->Parent->key[i];
					this->Parent->updateKey(this->key[0], this->LLeaf->key[LLeaf->size - 2]);
				}
				LLeaf->size--;
				this->size++;
			}
			//左节点分不出，看看右节点有没有
			else if (RLeaf && RLeaf->Parent == this->Parent && RLeaf->size > (_order - 1) / 2 + 1)
			{
				//从右兄弟取一个节点过来，右兄弟需更新key
				//本节点不用更新
				if (this->Type == LEAF)
				{
					this->key[this->size] = RLeaf->key[0];
					this->ptr[this->size] = RLeaf->ptr[0];
					RLeaf->ptr[0] = nullptr;
					this->Parent->updateKey(RLeaf->key[0], RLeaf->key[1]);
				}
				else
				{
					this->ptr[this->size] = RLeaf->ptr[0];
					RLeaf->GetNodePointer(0)->Parent = this;
					RLeaf->ptr[0] = nullptr;
					int i = 0;
					for (; this->Parent->ptr[i] != this; i++)
						;
					this->key[this->size - 1] = this->Parent->key[i];
					this->Parent->updateKey(this->Parent->key[i], this->RLeaf->key[0]);
				}
				for (uint j = 1; j < RLeaf->size; j++)
				{
					RLeaf->key[j - 1] = RLeaf->key[j];
					RLeaf->ptr[j - 1] = RLeaf->ptr[j];
				}
				RLeaf->ptr[--RLeaf->size] = nullptr;
				this->size++;
			}
			else
			{
				_KTy nextKey;
				//左右节点都没有,需要合并
				if (LLeaf && LLeaf->Parent == this->Parent)
				{
					uint i = 0;
					for (; this->Parent->ptr[i] != LLeaf; i++)
						;
					nextKey = Parent->key[i];
					//可以与左节点合并
					for (int i = this->size; i >= 0; i--)
					{
						this->key[i + LLeaf->size] = this->key[i];
						this->ptr[i + LLeaf->size] = this->ptr[i];
					}
					for (uint i = 0; i < LLeaf->size; i++)
					{
						this->ptr[i] = LLeaf->ptr[i];
						this->key[i] = LLeaf->key[i];
						if (this->Type == NONLEAF)
							LLeaf->GetNodePointer(i)->Parent = this;
						LLeaf->ptr[i] = nullptr;
					}
					if (this->Type == NONLEAF)
						this->key[LLeaf->size - 1] = nextKey;
					this->size += LLeaf->size;
					//从链表中取出
					if (this->LLeaf->LLeaf)
						this->LLeaf->LLeaf->RLeaf = this;
					this->LLeaf = this->LLeaf->LLeaf;
				}
				else if (RLeaf && RLeaf->Parent == this->Parent)
				{
					uint i = 0;
					for (; Parent->ptr[i] != this; i++)
						;
					nextKey = Parent->key[i];
					//与右节点合并
					for (int i = RLeaf->size; i >= 0; i--)
					{
						RLeaf->key[i + this->size] = RLeaf->key[i];
						RLeaf->ptr[i + this->size] = RLeaf->ptr[i];
					}
					for (uint i = 0; i < this->size; i++)
					{
						RLeaf->ptr[i] = this->ptr[i];
						RLeaf->key[i] = this->key[i];
						if (this->Type == NONLEAF)
							this->GetNodePointer(i)->Parent = RLeaf;
						this->ptr[i] = nullptr;
					}
					if (this->Type == NONLEAF)
						RLeaf->key[this->size - 1] = nextKey;
					RLeaf->size += this->size;
					//从链表中取出
					this->RLeaf->LLeaf = this->LLeaf;
					if (this->LLeaf)
						this->LLeaf->RLeaf = this->RLeaf;
				}
				else
				{
					//都不可以合并，说明该节点的父亲节点若存在，仅有一个儿子，这种情况仅出现在该节点为根
					if (this->Parent)
						throw new std::exception("This node is not a root!");
					//根的修改需要交给外部完成，若this->size!=0,外部不需要做任何操作,否则释放内存并初始化root=nullptr
					return true;
				}
				//合并结束，删除父节点中的被合并节点
				this->Parent->erase(nextKey);
			}
		}

		return flag;
	}

};
