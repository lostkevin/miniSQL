#include "IndexManager.h"

IndexManager::IndexManager ()
{
}

IndexManager::~IndexManager ()
{
}

bool IndexManager::defineTree (TreeTYPE type)
{
	return false;
}

void IndexManager::dropIndex ()
{
}

bool IndexManager::save (string fileName)
{
	return false;
}

void IndexManager::load (string fileName)
{
}

template<typename _KTy>
inline IndexInfo IndexManager::find (_KTy key)
{
	return IndexInfo ();
}

template<typename _KTy>
IndexInfo IndexManager::find (_KTy min, _KTy max)
{
	return IndexInfo ();
}

template<typename _KTy>
void IndexManager::insert (_KTy key, IndexInfo data)
{
}

template<typename _KTy>
void IndexManager::erase (_KTy key)
{
}
