#pragma once
//这是公共定义文件
//有啥宏都可以丢这里
#include <string>
using namespace std;

#ifndef BYTE
#define BYTE unsigned char
#endif // !BYTE

#ifndef uint
#define uint unsigned int
#endif // !uint


class IndexInfo {
private:
	//块大小
	uint _size;
	//文件偏移量
	uint _fileOffset;
	friend class IndexManager;
public:
	friend istream & operator>>(istream & is, IndexInfo & info);
	IndexInfo () {
		_fileOffset = 0;
	}
	uint fileOffset () const {
		return _fileOffset;
	}
};

using namespace std;

//缓冲块数目
#define MAX_BLOCK_NUM 4096
//缓冲块大小
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 8192
#endif // !MAX_BLOCK_SIZE
//裸缓冲区大小4K*8K=32M，实际会比这更大
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * MAX_BLOCK_SIZE)
