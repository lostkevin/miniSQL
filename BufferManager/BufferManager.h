#pragma once
#include <string>
#include <map>
#include <vector>
#include "..\IndexManager\IndexManager.h"


//缓冲区必须支持多个文件
class BufferManager {
private:
	class Page {
	private:
		//为了保证Page能自行写回
		string fileName;
		//相对文件头的偏移量
		uint blockOffset;
	public:
		//指向块的指针
		BYTE* pBlock;
		const uint _blockSize = MAX_BLOCK_SIZE;
		bool IsDirty;
		Page (string fileName);
		//析构时应保存这个page
		~Page ();
	};

	class Buffer {
	private:
		struct Node {
			Page *page;
			uint L;
			uint R;
			//true时无法从堆中弹出，因此也无法写回
			bool PIN;
		};
		uint size;
		Node minHeap[MAX_BLOCK_NUM + 1];
		uint pinnedNum;

	public:
		Buffer ();
		~Buffer ();
		//将page入堆
		bool Enque (Page * page);
		//将最陈旧的page出堆并释放该页，如果所有block都pin=true，return false 否则 true
		//如果遇到pinned node 线性查找到一个unpinned node，swap后再deque
		bool Deque ();
		//设置pin状态，如设置true，对该节点下滤
		void setPinState (uint i, bool Pinned);
		//上滤，入堆时用
		void Up (uint i);
		//下滤
		void Down (uint i);
	};
	//块大小是BLOCK_SIZE，将所有数据块对齐到2^k,块大于BLOCK_SIZE时抛异常
	//大程要求一个tuple的size至多为8192 （256*32），因此缓冲页必须8K
	uint ROUND (uint blockSize);
	//缓冲区
	Buffer buffer;
	uint blockUsed;
	const uint maxBlock = MAX_BLOCK_NUM;
	//从disk中读取一个page到buffer中，自动保存被替换的page, 读取成功返回true
	bool Load (const string &fileName, IndexInfo info);
public:
	//读取某个数据块的信息，将信息写到result指针指向的内存
	void readRawData (const string& fileName, const IndexInfo &info, BYTE * result);
	//向indexinfo指向的位置写入size字节, 数据源为pData，若数据不在buffer中，需要读入
	void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData);
	//新建一个块，size为该数据块的大小，为0时使用文件头设置（=0时文件不存在抛异常）
	const IndexInfo createBlock (const string & fileName, uint size = 0);
	//删除
	void erase (const IndexInfo &info);
	//drop,删除文件，释放该文件的所有缓冲块
	void drop (const string & fileName);
};

