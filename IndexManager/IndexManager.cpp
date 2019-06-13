#include <fstream>
#include "IndexManager.h"
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 8192
#endif // !MAX_BLOCK_SIZE

using namespace std;

IndexManager::IndexManager ()
{
	defined = false;
	BPTree = nullptr;
	treeType = INT; //无所谓
}

IndexManager::~IndexManager  ()
{
	if (BPTree) {
		//if (!defined)throw new exception ("Exist a tree without keytype defined!");
		switch (treeType) {
		case INT: {
			((IntTree*)BPTree)->~BPlusTree ();
			break;
		}
		case FLOAT: {
			((FloatTree*)BPTree)->~BPlusTree ();
			break;
		}
		case STRING: {
			((CharTree*)BPTree)->~BPlusTree ();
			break;
		}
		}
	}
}

bool IndexManager::defineTree (TreeTYPE type)
{
	if(defined)return false;
	this->treeType = type;
	if (treeType == INT) {
		BPTree = new IntTree (getNodeSize());
	}
	else if (treeType == FLOAT) {
		BPTree = new FloatTree (getNodeSize ());
	}
	else {
		BPTree = new CharTree (getNodeSize ());
	}
	return defined = true;
}

void IndexManager::dropIndex ()
{
	if (BPTree) {
		if (!defined)throw new exception ("Exist a tree without keytype defined!");
		switch (treeType) {
		case INT: {
			((IntTree*)BPTree)->~BPlusTree ();
			break;
		}
		case FLOAT: {
			((FloatTree*)BPTree)->~BPlusTree ();
			break;
		}
		case STRING: {
			((CharTree*)BPTree)->~BPlusTree ();
			break;
		}
		}
	}
	this->BPTree = nullptr;
	this->defined = false;
}

bool IndexManager::save (const string & fileName)
{
	if (defined) {
		if (!BPTree)throw new exception ("defined but do not exist a tree");
		ofstream fs = ofstream (fileName, ios::binary);
		if (!fs)return false;
		//第一行 记录数n keyType
		uint size = this->getSize ();
		fs << size << " " << treeType << endl;
		//2-n+1行 DataInfo Key
		for (uint i = 0; i < size; i++) {
			if (treeType == INT) {
				IntTree* ITree = (IntTree*)BPTree;
				vector<IntTree::Pair> v;
				ITree->getAllPair (v);
				for (uint i = 0; i < v.size (); i++) {
					fs << v[i].first << " " << v[i].second << endl;
				}
			}
			else if (treeType == FLOAT) {
				FloatTree* FTree = (FloatTree*)BPTree;
				vector<FloatTree::Pair> v;
				FTree->getAllPair (v);
				for (uint i = 0; i < v.size (); i++) {
					fs << v[i].first << " " << v[i].second << endl;
				}
			}
			else {
				CharTree* CTree = (CharTree*)BPTree;
				vector<CharTree::Pair> v;
				CTree->getAllPair (v);
				for (uint i = 0; i < v.size (); i++) {
					fs << v[i].first << " " << v[i].second << endl;
				}
			}
		}
		return true;
	}
	else return false;
}

bool IndexManager::load (const string & fileName)
{
	ifstream fs = ifstream(fileName, ios::binary);
	if (!fs)return false;
	uint size;
	uint type;
	fs >> size >> type;
	treeType = (TreeTYPE)type;
	for (uint i = 0; i < size; i++) {
		if (treeType == INT) {
			IntTree* ITree = (IntTree*)BPTree;
			IntTree::Pair p;
			for (uint i = 0; i < size; i++) {
				fs >> p.first >> p.second;
				ITree->insert (p.first, p.second);
			}
		}
		else if (treeType == FLOAT) {
			FloatTree* FTree = (FloatTree*)BPTree;
			FloatTree::Pair p;
			for (uint i = 0; i < size; i++) {
				fs >> p.first >> p.second;
				FTree->insert (p.first, p.second);
			}
		}
		else {
			CharTree* CTree = (CharTree*)BPTree;
			CharTree::Pair p;
			for (uint i = 0; i < size; i++) {
				fs >> p.first >> p.second;
				CTree->insert (p.first, p.second);
			}
		}
	}
	return true;
}

uint IndexManager::getNodeSize ()
{
	return uint ();
}

template<typename _KTy>
inline const IndexInfo IndexManager::find (_KTy key)
{
	if (defined) {
		if (!BPTree)throw new exception ("defined but do not exist a tree");
		if (treeType == INT) {
			IntTree* ITree = (IntTree*)BPTree;
			return *ITree->find (key);
		}
		else if (treeType == FLOAT) {
			FloatTree* FTree = (FloatTree*)BPTree;
			return *FTree->find (key);
		}
		else {
			CharTree* CTree = (CharTree*)BPTree;
			return *CTree->find (key);
		}
	}
	else throw new exception ("Exist a tree without keytype defined!");
}

template<typename _KTy>
inline const vector<IndexInfo> IndexManager::find (_KTy min, _KTy max)
{
	if (defined) {
		if (!BPTree)throw new exception ("defined but do not exist a tree");
		vector<IndexInfo> res;
		if (treeType == INT) {
			IntTree* ITree = (IntTree*)BPTree;
			vector<IndexInfo*> v = ITree->find (min, max);
			for (uint i = 0; i < v.size (); i++) {
				res.push_back (*v[i]);
			}
		}
		else if (treeType == FLOAT) {
			FloatTree* FTree = (FloatTree*)BPTree;
			vector<IndexInfo*> v = FTree->find (min, max);
			for (uint i = 0; i < v.size (); i++) {
				res.push_back (*v[i]);
			}
		}
		else {
			CharTree* CTree = (CharTree*)BPTree;
			vector<IndexInfo*> v = CTree->find (min, max);
			for (uint i = 0; i < v.size (); i++) {
				res.push_back (*v[i]);
			}
		}
		return res;
	}
	else throw new exception ("Exist a tree without keytype defined!");
}

template<typename _KTy>
inline void IndexManager::insert (const _KTy & key, const IndexInfo & data)
{
	if (defined) {
		if (!BPTree)throw new exception ("defined but do not exist a tree");
		if (treeType == INT) {
			IntTree* ITree = (IntTree*)BPTree;
			ITree->insert (key, data);
		}
		else if (treeType == FLOAT) {
			FloatTree* FTree = (FloatTree*)BPTree;
			FTree->insert (key, data);
		}
		else {
			CharTree* CTree = (CharTree*)BPTree;
			CTree->insert (key, data);
		}
	}
	else throw new exception ("Exist a tree without keytype defined!");
}

template<typename _KTy>
inline bool IndexManager::erase (const _KTy & key)
{
	if (defined) {
		if (!BPTree)throw new exception ("defined but do not exist a tree");
		if (treeType == INT) {
			IntTree* ITree = (IntTree*)BPTree;
			return ITree->erase (key);
		}
		else if (treeType == FLOAT) {
			FloatTree* FTree = (FloatTree*)BPTree;
			return FTree->erase (key);
		}
		else {
			CharTree* CTree = (CharTree*)BPTree;
			return CTree->erase (key);
		}
	}
	else throw new exception ("Exist a tree without keytype defined!");
}

ostream & operator<<(ostream & os, const IndexInfo & info)
{
	os << info.fileOffset () << " " << info.fileName();
	return os;
}

istream & operator>>(istream & is, IndexInfo & info)
{

	is >> info._fileOffset >> info._fileName;
	info._loadState = false;
	return is;
}


