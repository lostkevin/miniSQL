#include<fstream>
#include "BufferManager.h"
using namespace std;

uint BufferManager::getFileSize (const string& fileName) {
	ifstream fin (fileName);
	fin.seekg (0, ios::end);
	uint tmp = (uint)fin.tellg ();
	if (tmp % 8192)throw new exception ("Error!");
	fin.close ();
	return tmp;
}


BufferManager::Buffer::Buffer ()
{
	size = 0;
}

BufferManager::Buffer::~Buffer ()
{
	//BufferManager生存期结束，将所有page写回磁盘
	for (uint i = 1; i <= size; i++) {
		minHeap[i]->~Page ();
	}
}

bool BufferManager::Buffer::Enque (Page * page)
{
	if(this->size == maxSize)return false;
	minHeap[++this->size] = page;
	return true;
}

BufferManager::Page* BufferManager::Buffer::Deque ()
{
	if (!size)return nullptr;
	if (minHeap[1]->PIN) {
		//重新整理堆
		uint fore = 1;
		uint back = 1;
		while (fore <= size) {
			if (!minHeap[fore]->PIN) {
				swap (minHeap[fore++], minHeap[back++]);
			}
			else {
				fore++;
			}
		}
	}
	if (minHeap[1]->PIN)return nullptr;
	Page* res = minHeap[1];
	minHeap[1] = minHeap[size];
	minHeap[size--] = nullptr;
	Down (1);
	return res;
}

void BufferManager::Buffer::drop (const string & fileName)
{
	uint deletedElementNum = 0;
	uint back = 1;
	uint fore = 1;
	while (fore <= size) {
		if (minHeap[fore]->getFileName () == fileName) {
			delete minHeap[fore];
			minHeap[fore++] = nullptr;
			deletedElementNum++;
		}
		else {
			if (back != fore) {
				minHeap[back++] = minHeap[fore];
				minHeap[fore++] = nullptr;
			}
		}
	}
	size -= deletedElementNum;
}

void BufferManager::Buffer::Down (uint i)
{
	//L 2 * i  / R 2 * i + 1
	if (2 * i + 1 <= size) {
		if (minHeap[2 * i]->PIN)Down (2 * i);
		if (minHeap[2 * i + 1]->PIN)Down (2 * i + 1);
		if (minHeap[2 * i]->PIN && minHeap[2 * i + 1]->PIN) {
			//这条路堵住了
			return;
		}
		else if (!minHeap[2 * i]->PIN && !minHeap[2 * i + 1]->PIN) {
			uint coin = rand () % 2;
			swap (minHeap[i], minHeap[2 * i + coin]);
			Down (2 * i + coin);
		}
		else if (minHeap[2 * i]->PIN) {
			swap (minHeap[i], minHeap[2 * i + 1]);
			Down (2 * i + 1);
		}
		else {
			swap (minHeap[i], minHeap[2 * i]);
			Down (2 * i);
		}
	}
	else if (2 * i <= size) {
		if (!minHeap[2 * i]->PIN) {
			swap (minHeap[i], minHeap[2 * i]);
		}
		else return;
	}
	else return;
}

BufferManager::Page::Page (const string &fileName, uint blockOffset)
{
	this->fileName = fileName;
	this->blockOffset = blockOffset;
	this->IsDirty = false;
	this->PIN = false;

	this->pBlock = new BYTE[this->_pageSize]{ 0 };
	ifstream fs (fileName, ios::binary);
	if (fs) {
		fs.seekg (blockOffset);
		fs.read (this->pBlock, this->_pageSize);
		uint readSize = (uint)fs.gcount ();
		if (readSize < this->_pageSize) {
			throw new exception ("Load Page Failed");
		}
	}
	else {
		//file not exist, do nothing 
	}
	fs.close ();
}

BufferManager::Page::~Page ()
{
	if (this->IsDirty) {
		fstream fs (fileName, ios::in|ios::out|ios::binary);
		if (fs) {
			fs.seekp (this->blockOffset);
			fs.write (this->pBlock, this->_pageSize);
		}
		fs.close ();
	}
	delete pBlock;
}

uint BufferManager::ROUND (uint blockSize)
{
	//将一个数据块的大小向上进位至2^k
	if (!blockSize)return 0;
	blockSize--;
	blockSize |= blockSize >> 1;
	blockSize |= blockSize >> 2;
	blockSize |= blockSize >> 4;
	blockSize |= blockSize >> 8;
	blockSize |= blockSize >> 16;
	blockSize++; 
	return blockSize;
}

bool BufferManager::Load (const string & fileName, const IndexInfo &info)
{
	if (info._fileOffset & 0x00001FFF)return false;
	if (this->IsFileExist (fileName)) {
		Page* ptr = new Page (fileName, info._fileOffset);
		if (buffer.size == buffer.maxSize) {
			if (Page* ptr = buffer.Deque ()) {
				BufferIndex[ptr->getFileName ()].erase (ptr->getBlockOffset ());
				if (!BufferIndex[ptr->getFileName ()].size ())
					BufferIndex.erase (ptr->getFileName ());
				delete ptr;
			}
			else return false;
		}
		if (this->buffer.Enque (ptr))
			return BufferIndex[fileName][info._fileOffset] = ptr;
		delete ptr;
	}
	return false;
}

BufferManager::Page * BufferManager::getPage (const string & fileName, const IndexInfo & info)
{
	uint baseOffset = (info._fileOffset >> 13) << 13;
	auto iter = BufferIndex.find (fileName);
	if (iter != BufferIndex.end ()) {
		auto innerIter = iter->second.find (baseOffset);
		if (innerIter != iter->second.end ()) return innerIter->second;
	}
	if (Load (fileName, IndexInfo (baseOffset))) {
		return BufferIndex[fileName][baseOffset];
	}
	return nullptr;
	
}

uint BufferManager::getFileBlockSize (const string & fileName)
{
	Page* ptr = getPage (fileName, IndexInfo ());
	if (ptr) {
		BYTE* pData = ptr->pBlock;
		//0x00-0x03 size
		return *(uint*)pData;
	}
	else return 0;
}

uint BufferManager::getFreeNode (const string & fileName)
{
	Page* ptr = getPage (fileName, IndexInfo ());
	if (ptr) {
		bool lastState[2];
		lastState[0] = ptr->PIN;
		ptr->PIN = true;
		BYTE* pData = ptr->pBlock;
		pData += 8;
		//0x08-0x0B nextFreeNode
		uint nextNode = *(uint*)pData;
		//维护freelist，读入buffer的页
		Page* nextPage = getPage (fileName, IndexInfo (nextNode));
		if (!nextPage)throw new exception ("Unknown Exception");
		lastState[1] = nextPage->PIN;
		nextPage->PIN = true;
		BYTE* nextPDATA = nextPage->pBlock;
		uint InnerOffset = nextNode - nextPage->getBlockOffset ();
		//移动指针
		nextPDATA += 8 + InnerOffset;
		*(uint*)pData = *(uint*)nextPDATA;
		//更新过pData，脏页
		ptr->setDirty ();
		nextPage->PIN = lastState[1];
		ptr->PIN = lastState[0];
		return nextNode;
	}
	return 0;
}

void BufferManager::AddFreeNode (const string & fileName, uint offset)
{
	Page* ptr = getPage (fileName, IndexInfo ());
	if (ptr) {
		Page* nextPage = getPage (fileName, IndexInfo (offset));
		if (!nextPage)
			throw new exception ("Unknown Exception");
		bool lastState[2] = { ptr->PIN, nextPage->PIN };
		nextPage->PIN = true;
		ptr->PIN = true;
		BYTE* pData = ptr->pBlock;
		pData += 8;
		//0x08-0x0B nextFreeNode
		uint nextNode = *(uint*)pData;
		//维护freelist，读入buffer的页
		BYTE* nextPDATA = nextPage->pBlock;
		uint InnerOffset = offset - nextPage->getBlockOffset ();
		//移动指针
		nextPDATA += 8 + InnerOffset;
		*(uint*)pData = offset;
		*(uint*)nextPDATA = nextNode;
		//设置脏页
		ptr->setDirty ();
		nextPage->setDirty ();
		nextPage->PIN = lastState[1];
		ptr->PIN = lastState[0];
	}
}

bool BufferManager::readRawData (const string & fileName, const IndexInfo & info, BYTE * result)
{
	uint baseOffset = (info._fileOffset >> 13) << 13;
	Page* ptr = getPage (fileName, info);
	if (!ptr || !result)return false;
	bool lastState = ptr->PIN;
	ptr->PIN = true;
	BYTE* pData = ptr->pBlock;
	pData += info._fileOffset - baseOffset;
	memcpy_s (result, info._size, pData, info._size);
	ptr->PIN = lastState;
	return true;
}

void BufferManager::WriteRawData (const string & fileName, const IndexInfo & info, const BYTE * pData)
{
	bool editFile = IsFileExist (fileName);
	BYTE tmp[16] = { 0 };
	uint baseOffset = (info._fileOffset >> 13) << 13;
	Page * ptr = getPage(fileName, info);
	bool lastState = ptr->PIN;
	ptr->PIN = true;
	BYTE* dst = ptr->pBlock;
	if (editFile && !baseOffset)memcpy_s (tmp, 16, dst, 16);
	ptr->setDirty ();
	dst += info._fileOffset - baseOffset;
	memcpy_s (dst, info._size, pData, info._size);
	if (editFile && !baseOffset)memcpy_s (dst, 16, tmp, 16);
	ptr->PIN = lastState;
}

const IndexInfo BufferManager::createBlock (const string & fileName, uint size)
{
	Page* ptr = getPage (fileName, IndexInfo ());
	if (ptr) {
		//0x00-0x03 size
		uint BlockSizeInFile = getFileBlockSize (fileName);
		uint nextFreeNodeOffset = getFreeNode (fileName);
		if (BlockSizeInFile) {
			//当文件头保存着size时，不使用传入的参数
			if (!nextFreeNodeOffset) {
				//无freenode，追加写入
				uint offset = getFileSize (fileName);
				ofstream fs (fileName, ios::app | ios::binary);
				if (fs) {
					BYTE* emptyPage = new BYTE[Page::_pageSize]{ 0 };
					emptyPage[Page::_pageSize - 1] = (BYTE)0xff;
					fs.write (emptyPage, Page::_pageSize);
					delete emptyPage;
				}
				else throw new exception ("Incorrect File Name!");
				fs.close ();
				uint blockSizeAligned = ROUND (BlockSizeInFile);
				uint blockNum = Page::_pageSize / blockSizeAligned;
				for (uint i = 0; i < blockNum; i++) {
					AddFreeNode (fileName, offset);
					offset += blockSizeAligned;
				}
				nextFreeNodeOffset = getFreeNode (fileName);
			}
			//取得一块
			return IndexInfo (BlockSizeInFile, nextFreeNodeOffset);
		}
	}
	if(size){
		ofstream fs (fileName);
		if (fs) {
			BYTE* header = new BYTE[Page::_pageSize]{ 0 };
			header[Page::_pageSize - 1] = (BYTE)0xff;
			*(uint*)header = size;
			fs.write (header, Page::_pageSize);
			delete header;
		}
		else throw new exception ("Incorrect File Name!");
		//刷新缓冲区，接下来可以使用getPage了
		fs.close ();
		uint offset = getFileSize (fileName);
		fs.open (fileName, ios::app);
		BYTE* emptyPage = new BYTE[Page::_pageSize]{ 0 };
		emptyPage[Page::_pageSize - 1] = (BYTE)0xff;
		fs.write (emptyPage, Page::_pageSize);
		delete emptyPage;
		fs.close ();
		uint blockSizeAligned = ROUND (size);
		uint blockNum = Page::_pageSize / blockSizeAligned;
		for (uint i = 0; i < blockNum; i++) {
			AddFreeNode (fileName, offset);
			offset += blockSizeAligned;
		}
		uint freeNodeoffset = getFreeNode (fileName);
		return IndexInfo (size, freeNodeoffset);
	}
	else throw new exception ("try to create a new file but do not set blocksize");
}

void BufferManager::erase (const string & fileName, const IndexInfo & info)
{
	AddFreeNode (fileName, info._fileOffset);
}

void BufferManager::drop (const string & fileName)
{
	buffer.drop (fileName);
	BufferIndex.erase (fileName);
	remove (fileName.c_str ());
}

bool BufferManager::IsFileExist (const string & fileName)
{
	return (bool)ifstream (fileName);
}

void BufferManager::setPageState (const string & fileName, const IndexInfo & info, bool state)
{
	Page* ptr = getPage (fileName, info);
	if (ptr)ptr->PIN = state;
}

