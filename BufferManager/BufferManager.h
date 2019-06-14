#pragma once
#include <string>
#include <map>
#include "..\miniSQL\Commonheader.h"

//缓冲区必须支持多个文件
class BufferManager {
private:
	class Page {
	private:
		//为了保证Page能自行写回
		string fileName;
		//相对文件头的偏移量
		uint blockOffset;
		bool IsDirty;
	public:
		//指向块的指针
		BYTE* pBlock;
		static const uint _pageSize = PAGE_SIZE;
		bool setDirty () {
			return this->IsDirty ? false : (this->IsDirty = true);
		}
		bool PIN;
		Page (const string &fileName, uint blockOffset);
		const string& getFileName () {
			return fileName;
		}
		const uint getBlockOffset () {
			return blockOffset;
		}
		//析构时应保存这个page
		~Page ();
	};

	class Buffer {
	private:
		
		Page* minHeap[MAX_BLOCK_NUM + 1];
		//下滤
		void Down (uint i);

	public:
		Buffer ();
		~Buffer ();
		//将page入堆
		bool Enque (Page * page);
		//将最陈旧的page出堆并释放该页，如果所有block都pin=true，return nullptr
		//如果遇到pinned node,重新整理堆后再deque
		Page* Deque ();
		//删除来自于指定文件的Page
		void drop (const string& fileName);
		uint size;
		const uint maxSize = MAX_BLOCK_NUM;
	};
	//块大小是BLOCK_SIZE，将所有数据块对齐到2^k,块大于BLOCK_SIZE时抛异常
	//大程要求一个tuple的size至多为8192 （256*32），因此缓冲页必须8K
	uint ROUND (uint blockSize);
	//缓冲区
	Buffer buffer;
	map<string, map<uint, Page*>> BufferIndex;
	//从disk中读取一个page到buffer中，自动保存被替换的page, 读取成功返回true
	bool Load (const string &fileName, const IndexInfo &info);
	//读取该IndexInfo的offset所在的Page
	Page* getPage (const string &fileName, const IndexInfo &info);
	uint getFileBlockSize (const string & fileName);
	//向freelist中取出节点
	uint getFreeNode (const string & fileName);
	//向freelist中添加数据
	void AddFreeNode (const string & fileName, uint offset);
public:
	//读取某个数据块的信息，将信息写到result指针指向的内存, 如果读取失败，返回false
	bool readRawData (const string& fileName, const IndexInfo &info, BYTE * result);
	//向indexinfo指向的位置写入size字节, 数据源为pData，若数据不在buffer中，需要读入
	void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData);
	//新建一个块，size为该数据块的大小，为0时使用文件头设置（=0时文件不存在抛异常）
	const IndexInfo createBlock (const string & fileName, uint size = 0);
	//删除某个块的数据，即将这个块加入到freelist内
	//如果文件不存在，什么都不做
	void erase (const string & fileName, const IndexInfo &info);
	//drop,删除文件，释放该文件的所有缓冲块
	void drop (const string & fileName);
	//判断文件是否存在
	bool IsFileExist (const string& fileName);
	void setPageState (const string &fileName, const IndexInfo &info, bool state);
};

