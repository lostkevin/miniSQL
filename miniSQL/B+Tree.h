#pragma once
#ifndef uint
#define uint unsigned int
#endif // !uint
#include <map>
using namespace std;

enum NodeType { LEAF, NONLEAF };

template<typename KeyType, typename DataType> class BPlusTree;
template<typename KeyType, typename DataType> class BPlusNode;
template<typename _Kty> using Elements = map<_Kty, void*>;


/*
使用STL map作为B+树Node内索引的储存
map示意图
NONLEAF
  Key1 < Key2 < Key3 < ... < KeyM-1  maxPtr
  <Key1  <Key2  <Key3 ...    <KeyM-1 >=KeyM-1
LEAF
	Key1 < Key2 ... < KeyM
	=Key1 =Key2 ...  =KeyM
相对于list(N是order)
list： 查询：O(N) 合并/分割：O(N)(需要查找)
map: 查询：O(lgN) 合并/分割：O(NlgN)
*/
template<typename KeyType, typename DataType>
class BPlusNode {
public:
	typedef BPlusNode<typename KeyType, typename DataType> Node;
	typedef Node* NodePtr;
	uint order;
	NodePtr maxPointer;
	Elements<KeyType> elements;
	//指向父节点的指针
	NodePtr Parent;
	NodeType type;
	BPlusNode (uint order) {
		this->maxPointer = nullptr;
		this->order = order;
		this->type = LEAF;
		this->Parent = nullptr;
	}
	BPlusNode ( ) :BPlusNode (4) {};
	DataType* find (typename KeyType key) {
		if (this->type == NONLEAF) {
			auto tmp = elements.upper_bound(key);
			if (tmp != elements.end ( )) {
				return ((Node*)(tmp->second))->find (key);
			} else {
				return maxPointer->find(key);
			}
		} else {
			auto tmp = elements.lower_bound (key);
			if (tmp != elements.end ( ) && tmp->first == key) {
				return (DataType*)(tmp->second);
			} else return nullptr;
		}
	}

private:
};

//每个Node包含一个key和一个ptr，若B+Node非叶，Node的ptr指向子节点，且子节点的max Key < Node.key
//若B+Node为叶，除最后一个节点外，每个ptr均指向数据，最后一个ptr指向Rsibling
//需要排序，重载operator <

//需要支持的方法:search,insert,erase
//不需要update方法
template<typename KeyType, typename DataType>
class BPlusTree {
public:
	typedef BPlusNode<KeyType, DataType> Node;
	typedef Node* NodePtr;

	
	BPlusTree (uint order) {
		this->order = order;
		BPTree = new BPlusNode<KeyType, DataType> (order);
	}
	BPlusTree ( ):BPlusTree(4){}

	void insert (KeyType &&key, DataType *&&data);

	//delete node in template & free data
	bool erase (KeyType key);

	DataType* find (KeyType key) {
		return BPTree ? BPTree->find (key) : nullptr;
	}
private:
	uint order;
	NodePtr BPTree;
};

template<typename KeyType, typename DataType>
inline void BPlusTree<KeyType, DataType>::insert (KeyType && key, DataType *&& data) {
	{
		auto ptr = this->BPTree;
		while (ptr->type == NONLEAF) {
			auto tmp = ptr->elements.lower_bound (key);
			if (tmp != ptr->elements.end ( )) {
				if (tmp->first == key)throw new exception ("The Key Has Been Inserted!");
				ptr = (NodePtr)(tmp->second);
			} else {
				ptr = ptr->maxPointer;
			}
		}
		//LEAF Node found
		auto tmp = ptr->elements.lower_bound (key);
		if (tmp != ptr->elements.end ( )) {
			if (tmp->first == key)throw new exception ("The Key Has Been Inserted!");
			if (tmp == ptr->elements.begin ( )) {
				//新插入的数据较小，从leaf向根更新节点的key
				auto updater = ptr;
				while (updater->Parent) {
					//用本节点最小key在父节点中查找，得到迭代器
					typename Elements<KeyType>::iterator iter = updater->Parent->elements.find (updater->elements.begin ( )->first);
					if (iter != updater->Parent->elements.end ( )) {
						//取出
						auto node = updater->elements.extract (iter);
						//更新key
						node.key ( ) = tmp->first;
						//重新插入
						updater->Parent->elements.insert (move (node));
					}
					updater = updater->Parent;
				}
			}
			ptr->elements.insert (pair<KeyType, void*> (key, data));
		} else {
			ptr->elements.insert (pair<KeyType, void*> (key, data));
			//不用更新了，for all element in ptr->elements, element.key < key;
		}
		//size == order + 1, 需要分裂
		if (ptr->elements.size ( ) == order + 1) {
			BPlusNode<KeyType, DataType>* newNode = new BPlusNode<KeyType, DataType> (order);
			uint size = order + 1;
			auto iter = ptr->elements.end ( );
			--iter;
			while (ptr->elements.size ( ) > size / 2) {
				auto pObjRemoved = iter--;
				auto tmp = ptr->elements.extract (pObjRemoved);
				newNode->elements.insert (move (tmp));
			}
			//newNode接在ptr后面
			newNode->maxPointer = ptr->maxPointer;
			ptr->maxPointer = newNode;
			newNode->Parent = ptr->Parent;
			auto InsertedPair = pair<KeyType, void*> (KeyType ( ), newNode);
			if (ptr->Parent) {
				//更新ptr的key
				auto ParentIter = ptr->Parent->elements.find (ptr->elements.begin ( )->first);
				if (ParentIter == ptr->Parent->elements.end ( )) {
					//最小节点
					ParentIter = ptr->Parent->elements.begin ( );
					//取出
					auto node = ptr->Parent->elements.extract (ParentIter);
					//更新key
					InsertedPair.first = node.key ( );
					node.key ( ) = newNode->elements.begin ( )->first;
					//重新插入
					ptr->Parent->elements.insert (move (node));
				} else if (++ParentIter != ptr->Parent->elements.end ( )) {
					//取出
					auto node = ptr->Parent->elements.extract (ParentIter);
					//更新key
					InsertedPair.first = node.key ( );
					node.key ( ) = newNode->elements.begin ( )->first;
					//重新插入
					ptr->Parent->elements.insert (move (node));
				} else {
					//最大节点
					InsertedPair.second = ptr;
					InsertedPair.first = newNode->elements.begin ( )->first;
					ptr->Parent->maxPointer = newNode;
				}

				while (ptr->Parent) {
					ptr = ptr->Parent;
					ptr->elements.insert (InsertedPair);
					if (ptr->elements.size ( ) == order) {
						//内节点分裂
						newNode = new BPlusNode<KeyType, DataType> (order);
						uint size = ptr->elements.size ( );
						auto iter = ptr->elements.begin ( );
						while (ptr->elements.size ( ) > size / 2) {
							auto pObjRemoved = iter++;;
							newNode->elements.insert (move (ptr->elements.extract (pObjRemoved)));
						}
						newNode->Parent = ptr->Parent;
						newNode->type = NONLEAF;
						for (auto iter = newNode->elements.begin ( ); iter != newNode->elements.end ( ); iter++) {
							((NodePtr)iter->second)->Parent = newNode;
						}
						typename Elements<KeyType>::iterator maxItem = newNode->elements.end ( );
						maxItem--;
						newNode->maxPointer = (NodePtr)maxItem->second;
						//创建新pair
						InsertedPair = pair<KeyType, void*> (maxItem->first, newNode);
						newNode->elements.erase (maxItem);
					} else return;
				}
				if (!ptr->Parent) {
					//升高一层
					BPTree = new BPlusNode<KeyType, DataType> (order);
					BPTree->type = NONLEAF;
					BPTree->maxPointer = ptr;
					ptr->Parent = BPTree;
					newNode->Parent = BPTree;
					BPTree->elements.insert (InsertedPair);
				}
				return;
			} else {
				//升高一层
				BPTree = new BPlusNode<KeyType, DataType> (order);
				BPTree->type = NONLEAF;
				BPTree->maxPointer = newNode;
				InsertedPair.first = newNode->elements.begin ( )->first;
				InsertedPair.second = ptr;
				ptr->Parent = BPTree;
				newNode->Parent = BPTree;
				BPTree->elements.insert (InsertedPair);
			}
		}
	}
}

template<typename KeyType, typename DataType>
inline bool BPlusTree<KeyType, DataType>::erase (KeyType key) {

	return false;
}
