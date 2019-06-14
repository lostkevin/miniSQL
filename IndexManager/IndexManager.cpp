#include <fstream>
#include "IndexManager.h"


uint IndexManager::calcOrder ( uint KeySize)
{
	uint pairSize = sizeof (IndexInfo) + KeySize;
	uint freeSize = PAGE_SIZE - 3 * sizeof(IndexInfo) - sizeof (uint);
	return freeSize / pairSize - 1;
}

bool IndexManager::fail ()
{
	return (this->type != UNDEF) && ITree;
}

bool IndexManager::setIndexInfo (TreeTYPE type, uint keySize)
{
	if(this->type != UNDEF || type == UNDEF)return false;
	BYTE header[PAGE_SIZE];
	BYTE* ptr = header;
	*(uint*)ptr = PAGE_SIZE;
	ptr += 16;
	//0x10-0x17 索引类型（int索引，float索引，string索引）
	*(TreeTYPE*)ptr = this->type = type;
	switch (type) {
	case INT: {
		ITree = new Index<int> ();
		if (keySize != 4)throw new exception ("incorrect size");
		break;
	}
	case FLOAT: {
		FTree = new Index<float> ();
		if (keySize != 4)throw new exception ("incorrect size");
		break;
	}
	case STRING: {
		CTree = new Index<string> ();
		if (keySize  <= 1 || keySize > 256)throw new exception ("incorrect size");
		break;
	}
	}
	ptr += sizeof (TreeTYPE);
	*(uint*)ptr = keySize;
	ptr += sizeof (uint);
	*(uint*)ptr = calcOrder (keySize);
	IOManager.WriteRawData (IndexInfo (), header);
	return true;
}

IndexManager::IndexManager (BufferManager & bufferMgr):IOManager (bufferMgr) 
{
	type = UNDEF;
	ITree = nullptr;
}

IndexManager::IndexManager (const string & fileName, BufferManager & bufferMgr):IOManager (fileName, bufferMgr)
{
	if (bufferMgr.IsFileExist (fileName)) {
		BYTE header[PAGE_SIZE];
		bufferMgr.readRawData (fileName, IndexInfo (), header);
		BYTE* ptr = header;
		ptr += 16;
		//0x10-0x17 索引类型（int索引，float索引，string索引）
		type = *(TreeTYPE*)ptr;
		switch (type) {
		case INT: {
			ITree = new Index<int> ();
			break;
		}
		case FLOAT: {
			FTree = new Index<float>();
			break;
		}
		case STRING: {
			CTree = new Index<string>();
			break;
		}
		default:
			//File Exist But Type Undef, error
			ITree = nullptr;
		}
	}
	else {
		type = UNDEF;
		ITree = nullptr;
	}
}

IndexManager::~IndexManager  ()
{
	switch (type) {
	case INT: {
		delete ITree;
		break;
	}
	case FLOAT: {
		delete FTree;
		break;
	}
	case STRING: {
		delete CTree;
		break;
	}
	}
}

bool IndexManager::open (const string & fileName)
{
	if (IOManager.open (fileName)) {
		BYTE header[PAGE_SIZE];
		IOManager.ReadRawData (IndexInfo (), header);
		BYTE* ptr = header;
		ptr += 16;
		//0x10-0x17 索引类型（int索引，float索引，string索引）
		type = *(TreeTYPE*)ptr;
		switch (type) {
		case INT: {
			ITree = new Index<int> ();
			break;
		}
		case FLOAT: {
			FTree = new Index<float> ();
			break;
		}
		case STRING: {
			CTree = new Index<string> ();
			break;
		}
		default:
			//File Exist But Type Undef, error
			ITree = nullptr;
			return false;
		}
	}
	else {
		type = UNDEF;
		ITree = nullptr;
	}
	return true;
}

void IndexManager::close ()
{
	switch (type) {
	case INT: {
		delete ITree;
		break;
	}
	case FLOAT: {
		delete FTree;
		break;
	}
	case STRING: {
		delete CTree;
		break;
	}
	}
	IOManager.close ();
	type = UNDEF;
	ITree = nullptr;
}

void IndexManager::dropIndex ()
{
	IOManager.drop ();
	type = UNDEF;
	ITree = nullptr;
}

template<typename _Ty>
inline const IndexInfo IndexManager::find (_Ty key)
{
	switch (type) {
	case INT: {
		static_assert(!Conversion<_Ty, int>::state, "Key Type Incorrect!");
		return ITree->find (key);
	}
	case FLOAT: {
		static_assert(!Conversion<_Ty, float>::state, "Key Type Incorrect!");
		return FTree->find (key);
	}
	case STRING: {
		static_assert(!Conversion<_Ty, string>::state, "Key Type Incorrect!");
		return CTree->find (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<typename _Ty>
inline const vector<IndexInfo> IndexManager::find (_Ty min, _Ty max)
{
	switch (type) {
	case INT: {
		static_assert(!Conversion<_Ty, int>::state, "Key Type Incorrect!");
		return ITree->find (min, max);
	}
	case FLOAT: {
		static_assert(!Conversion<_Ty, float>::state, "Key Type Incorrect!");
		return FTree->find (min, max);
	}
	case STRING: {
		static_assert(!Conversion<_Ty, string>::state, "Key Type Incorrect!");
		return CTree->find (min, max);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<typename _Ty>
inline void IndexManager::insert (const _Ty & key, const IndexInfo & data)
{
	switch (type) {
	case INT: {
		static_assert(!Conversion<_Ty, int>::state, "Key Type Incorrect!");
		ITree->insert (key, data);
		break;
	}
	case FLOAT: {
		static_assert(!Conversion<_Ty, float>::state, "Key Type Incorrect!");
		FTree->insert (key, data);
		break;
	}
	case STRING: {
		static_assert(!Conversion<_Ty, string>::state, "Key Type Incorrect!");
		CTree->insert (key, data);
		break;
	}
	default:
		throw new exception ("NullIndexException");
	}
}

template<typename _Ty>
inline bool IndexManager::erase (const _Ty & key)
{
	switch (type) {
	case INT: {
		static_assert(!Conversion<_Ty, int>::state, "Key Type Incorrect!");
		return ITree->erase (key);
	}
	case FLOAT: {
		static_assert(!Conversion<_Ty, float>::state, "Key Type Incorrect!");
		return FTree->erase (key);
	}
	case STRING: {
		static_assert(!Conversion<_Ty, string>::state, "Key Type Incorrect!");
		return CTree->erase (key);
	}
	default:
		throw new exception ("NullIndexException");
	}
}

bool IndexManager::BufferIO::ReadRawData (const IndexInfo & info, BYTE (&rawData)[PAGE_SIZE])
{
	bufferMgr.setPageState (fileName, info, true);
	return bufferMgr.readRawData (fileName, info, rawData);
}

bool IndexManager::BufferIO::WriteRawData (const IndexInfo & info, const BYTE (&rawData)[PAGE_SIZE])
{
	ofstream fs (fileName);
	if (!fs)return false;
	fs.close ();
	bufferMgr.setPageState (fileName, info, false);
	bufferMgr.WriteRawData (fileName, info, rawData);
}

void IndexManager::BufferIO::drop ()
{
	bufferMgr.drop (fileName);
}

void IndexManager::BufferIO::close ()
{
	fileName = string ();
}

bool IndexManager::BufferIO::open (const string & fileName)
{
	this->fileName = fileName;
	return bufferMgr.IsFileExist (fileName);
}

void IndexManager::BufferIO::release (const IndexInfo & info)
{
	bufferMgr.setPageState (fileName, info, false);
}
