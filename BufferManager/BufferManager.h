#pragma once
#include <string>
#include "stdlib.h"
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
//极限利用率为~50%
//删除时不会减少数据文件体积（仅修改索引和freelist）
//一个table一个文件，便于储存

//缓冲区必须支持不同table的同时读写，因此buffer内部保存tuple的size
class BufferManager {
private:
	class Buffer {
	private:
		string fileName;

		uint tupleSize;
		bool PIN;
		//相对文件头的偏移量
		uint blockOffset;
		//块大小是BLOCK_SIZE，将所有tuple对齐到2^k,tuple大于BLOCK_SIZE时抛异常
		//大程要求一个tuple的size至多为8192 （256*32）
		uint ROUND ();
	public:
		BYTE* pBlock;
		const uint _blockSize = MAX_BLOCK_SIZE;
		bool IsDirty;
		Buffer ();
		Buffer (const string &fileName);
		~Buffer ();

	};

	//缓冲区
	Buffer* BufferRegion;
	uint blockUsed;
	const uint maxBlock = MAX_BLOCK_NUM;
	//保存所有block到disk中,不返回值，失败抛异常
	void SaveAll ();
	//从disk中读取一个block到buffer中，自动保存被替换的buffer, 读取成功返回true
	//需要给出一个tuple的size
	bool Load (const string &fileName, uint tupleSize);
	//新增一个空tuple，若文件名不存在，新建
	const IndexInfo newTuple (const string &fileName);
public:
	//create tuple, 新建一个tuple，将pData指针指向的值复制到这个tuple里
	//如果文件不存在，新建文件
	//文件存在且freelist非空，打开freelist指向的位置对应的block
	//文件存在且freelist为空，向新建buffer，buffer指向文件尾部
	//返回该tuple的索引
	const IndexInfo createTuple (const string & fileName, const BYTE*result, const uint size);
	//读取某个tuple的信息，将信息写到result指针指向的内存
	void readData (const IndexInfo &info, BYTE * result, const uint size);
	//修改tuple，为了方便实现，使用字节直接写入,从PData向buffer指定位置写入size bytes
	void Write (const IndexInfo &info, const BYTE* pData, uint size);
	//删除，将该块加入freelist，需访问0x00块和待删除的块
	void eraseTuple (const IndexInfo &info);
	//drop,删除整个table的数据
	void dropTable (const string & fileName);
};

