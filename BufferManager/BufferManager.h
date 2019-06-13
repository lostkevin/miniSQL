#pragma once
#include <string>
#include <map>
#include <vector>
#include "..\IndexManager\IndexManager.h"
using namespace std;

//缓冲块数目
#define MAX_BLOCK_NUM 4096
//缓冲块大小
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 8192
#endif // !MAX_BLOCK_SIZE
//缓冲区大小4K*8K=32M
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * MAX_BLOCK_SIZE)

#ifndef BYTE
#define BYTE signed char
#endif

//table储存格式
//0x00 保存freelist，不保存数据
//之后的偏移量按照2^k对齐
//除对齐的空间以外至多消耗BLOCK_SIZE
//删除时仅修改索引和freelist
//一个table一个文件，便于储存

//缓冲区必须支持多个文件
class BufferManager {
private:
	class Page {
	private:
		//为了保证Page能自行写回
		string fileName;
		bool PIN; //true时无法写回到文件
		//相对文件头的偏移量
		uint blockOffset;
		//块大小是BLOCK_SIZE，将所有数据块对齐到2^k,块大于BLOCK_SIZE时抛异常
		//大程要求一个tuple的size至多为8192 （256*32），因此缓冲页必须8K
		uint ROUND ();
	public:
		//指向块的指针
		BYTE* pBlock;
		const uint _blockSize = MAX_BLOCK_SIZE;
		bool IsDirty;
		Page ();
		//析构时应保存这个page
		~Page ();
		void setFree ();
	};

	//缓冲区
	Page* Buffer;
	uint blockUsed;
	const uint maxBlock = MAX_BLOCK_NUM;
	//从disk中读取一个page到buffer中，自动保存被替换的page, 读取成功返回true
	bool Load (const string &fileName, IndexInfo info);
	//如果文件不存在，新建文件
	//文件存在且freelist非空，打开freelist指向的位置对应的block
	//文件存在且freelist为空，向新建buffer，buffer指向文件尾部
	//返回该Page的索引
	const IndexInfo createPage (const string & fileName);
public:
	//读取某个数据块的信息，将信息写到result指针指向的内存
	void readRawData (const string& fileName, const IndexInfo &info, BYTE * result);
	//向indexinfo指向的位置写入size字节, 数据源为pData，若数据不在buffer中，需要读入
	void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData);
	//删除
	void erase (const IndexInfo &info);
	//drop,删除文件，释放该文件的所有缓冲块
	void drop (const string & fileName);
};

