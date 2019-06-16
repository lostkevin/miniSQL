#include <fstream>
#include "IndexManager.h"


uint IndexManager::calcOrder ( uint KeySize)
{
	uint pairSize = sizeof (IndexInfo) + KeySize;
	uint freeSize = PAGE_SIZE - 3 * sizeof(IndexInfo) - sizeof (uint) - sizeof(NodeType);
	return freeSize / pairSize - 1;
}

bool IndexManager::fail ()
{
	return (this->type != UNDEF) && ITree;
}

bool IndexManager::setIndexInfo (TreeTYPE type, uint keySize)
{
	IndexInfo newPage = IOManager.NewPage ();
	IOManager.erase (newPage);
	if(this->type != UNDEF || type == UNDEF)return false;
	BYTE header[PAGE_SIZE] = { 0 };
	BYTE* ptr = header;
	*(uint*)ptr = PAGE_SIZE;
	ptr += 16;
	//0x10-0x17 索引类型（int索引，float索引，string索引）
	*(TreeTYPE*)ptr = this->type = type;
	switch (type) {
	case INT: {
		ITree = new Index<int> (IOManager);
		if (keySize != 4)throw new exception ("incorrect size");
		break;
	}
	case FLOAT: {
		FTree = new Index<float> (IOManager);
		if (keySize != 4)throw new exception ("incorrect size");
		break;
	}
	case STRING: {
		CTree = new Index<string> (IOManager);
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
			ITree = new Index<int> (IOManager);
			break;
		}
		case FLOAT: {
			FTree = new Index<float>(IOManager);
			break;
		}
		case STRING: {
			CTree = new Index<string>(IOManager);
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
			ITree = new Index<int> (IOManager);
			break;
		}
		case FLOAT: {
			FTree = new Index<float> (IOManager);
			break;
		}
		case STRING: {
			CTree = new Index<string> (IOManager);
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

bool BufferIO::ReadRawData (const IndexInfo & info, BYTE (&rawData)[PAGE_SIZE])
{
	bufferMgr.setPageState (fileName, info, true);
	return bufferMgr.readRawData (fileName, info, rawData);
}

bool BufferIO::WriteRawData (const IndexInfo & info, const BYTE (&rawData)[PAGE_SIZE])
{
	fstream fs (fileName, ios::in | ios::out | ios::binary);
	if (!fs)return false;
	fs.close ();
	bufferMgr.setPageState (fileName, info, false);
	bufferMgr.WriteRawData (fileName, info, rawData);
	return true;
}

const IndexInfo BufferIO::NewPage ()
{
	return bufferMgr.createBlock (fileName, PAGE_SIZE);
}

void BufferIO::erase (const IndexInfo & info)
{
	bufferMgr.erase(fileName, info);
}

void BufferIO::drop ()
{
	bufferMgr.drop (fileName);
}

void BufferIO::close ()
{
	fileName = string ();
}

bool BufferIO::open (const string & fileName)
{
	this->fileName = fileName;
	return bufferMgr.IsFileExist (fileName);
}

void BufferIO::release (const IndexInfo & info)
{
	bufferMgr.setPageState (fileName, info, false);
}
