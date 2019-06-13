#include "BufferManager.h"

BufferManager::Buffer::Buffer ()
{
}

BufferManager::Buffer::~Buffer ()
{
}

bool BufferManager::Buffer::Enque (Page * page)
{
	return false;
}

bool BufferManager::Buffer::Deque ()
{
	return false;
}

void BufferManager::Buffer::setPinState (uint i, bool Pinned)
{
}

void BufferManager::Buffer::Up (uint i)
{
}

void BufferManager::Buffer::Down (uint i)
{
}

BufferManager::Page::Page (string fileName)
{
}

BufferManager::Page::~Page ()
{
}


uint BufferManager::ROUND (uint blockSize)
{
	return uint ();
}

bool BufferManager::Load (const string & fileName, IndexInfo info)
{
	return false;
}

void BufferManager::readRawData (const string & fileName, const IndexInfo & info, BYTE * result)
{
}

void BufferManager::WriteRawData (const string & fileName, const IndexInfo & info, const BYTE * pData)
{
}

const IndexInfo BufferManager::createBlock (const string & fileName, uint size = 0)
{
	return IndexInfo ();
}

void BufferManager::erase (const IndexInfo & info)
{
}

void BufferManager::drop (const string & fileName)
{
}
