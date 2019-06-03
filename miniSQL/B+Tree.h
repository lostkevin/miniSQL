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
ʹ��STL map��ΪB+��Node�������Ĵ���
mapʾ��ͼ
NONLEAF
  Key1 < Key2 < Key3 < ... < KeyM-1  maxPtr
  <Key1  <Key2  <Key3 ...    <KeyM-1 >=KeyM-1
LEAF
	Key1 < Key2 ... < KeyM
	=Key1 =Key2 ...  =KeyM
�����list(N��order)
list�� ��ѯ��O(N) �ϲ�/�ָO(N)(��Ҫ����)
map: ��ѯ��O(lgN) �ϲ�/�ָO(NlgN)
*/
template<typename KeyType, typename DataType>
class BPlusNode {
public:
	typedef BPlusNode<typename KeyType, typename DataType> Node;
	typedef Node* NodePtr;
	uint order;
	NodePtr maxPointer;
	Elements<KeyType> elements;
	//ָ�򸸽ڵ��ָ��
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

//ÿ��Node����һ��key��һ��ptr����B+Node��Ҷ��Node��ptrָ���ӽڵ㣬���ӽڵ��max Key < Node.key
//��B+NodeΪҶ�������һ���ڵ��⣬ÿ��ptr��ָ�����ݣ����һ��ptrָ��Rsibling
//��Ҫ��������operator <

//��Ҫ֧�ֵķ���:search,insert,erase
//����Ҫupdate����
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
				//�²�������ݽ�С����leaf������½ڵ��key
				auto updater = ptr;
				while (updater->Parent) {
					//�ñ��ڵ���Сkey�ڸ��ڵ��в��ң��õ�������
					typename Elements<KeyType>::iterator iter = updater->Parent->elements.find (updater->elements.begin ( )->first);
					if (iter != updater->Parent->elements.end ( )) {
						//ȡ��
						auto node = updater->elements.extract (iter);
						//����key
						node.key ( ) = tmp->first;
						//���²���
						updater->Parent->elements.insert (move (node));
					}
					updater = updater->Parent;
				}
			}
			ptr->elements.insert (pair<KeyType, void*> (key, data));
		} else {
			ptr->elements.insert (pair<KeyType, void*> (key, data));
			//���ø����ˣ�for all element in ptr->elements, element.key < key;
		}
		//size == order + 1, ��Ҫ����
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
			//newNode����ptr����
			newNode->maxPointer = ptr->maxPointer;
			ptr->maxPointer = newNode;
			newNode->Parent = ptr->Parent;
			auto InsertedPair = pair<KeyType, void*> (KeyType ( ), newNode);
			if (ptr->Parent) {
				//����ptr��key
				auto ParentIter = ptr->Parent->elements.find (ptr->elements.begin ( )->first);
				if (ParentIter == ptr->Parent->elements.end ( )) {
					//��С�ڵ�
					ParentIter = ptr->Parent->elements.begin ( );
					//ȡ��
					auto node = ptr->Parent->elements.extract (ParentIter);
					//����key
					InsertedPair.first = node.key ( );
					node.key ( ) = newNode->elements.begin ( )->first;
					//���²���
					ptr->Parent->elements.insert (move (node));
				} else if (++ParentIter != ptr->Parent->elements.end ( )) {
					//ȡ��
					auto node = ptr->Parent->elements.extract (ParentIter);
					//����key
					InsertedPair.first = node.key ( );
					node.key ( ) = newNode->elements.begin ( )->first;
					//���²���
					ptr->Parent->elements.insert (move (node));
				} else {
					//���ڵ�
					InsertedPair.second = ptr;
					InsertedPair.first = newNode->elements.begin ( )->first;
					ptr->Parent->maxPointer = newNode;
				}

				while (ptr->Parent) {
					ptr = ptr->Parent;
					ptr->elements.insert (InsertedPair);
					if (ptr->elements.size ( ) == order) {
						//�ڽڵ����
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
						//������pair
						InsertedPair = pair<KeyType, void*> (maxItem->first, newNode);
						newNode->elements.erase (maxItem);
					} else return;
				}
				if (!ptr->Parent) {
					//����һ��
					BPTree = new BPlusNode<KeyType, DataType> (order);
					BPTree->type = NONLEAF;
					BPTree->maxPointer = ptr;
					ptr->Parent = BPTree;
					newNode->Parent = BPTree;
					BPTree->elements.insert (InsertedPair);
				}
				return;
			} else {
				//����һ��
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
