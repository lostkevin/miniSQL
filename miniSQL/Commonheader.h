#pragma once
//这是公共定义文件
//有啥宏都可以丢这里
#include <string>
using namespace std;
//缓冲块数目
#define MAX_BLOCK_NUM 0x1000
//缓冲块大小
//为了避免数据的跨页储存，极限是32*char（255）= 8192
//Index没有这个问题，一个pair至多260Bytes，所以一个节点至少能做到30-40扇出
#define PAGE_SIZE 0x2000
//裸缓冲区大小4K*8K=32M，实际会比这更大
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * PAGE_SIZE)
#define BYTE char
#define uint unsigned int


class IndexInfo {
private:
	//块大小
	uint _size;
	//文件偏移量
	uint _fileOffset;
	//friend class IndexManager;
	friend class BufferManager;
	friend int main ();
	template <typename _Ty>friend class Index;
	IndexInfo (uint size, uint fileOffset) {
		_fileOffset = fileOffset;
		_size = size;
	}

	IndexInfo (uint fileOffset) {
		_fileOffset = fileOffset;
	}
public:
	IndexInfo () {
		_fileOffset = 0;
		_size = PAGE_SIZE;
	}
	bool operator < (const IndexInfo& r) const{
		return this->_fileOffset < r._fileOffset;
	}

	bool operator == (const IndexInfo& r)const {
		return this->_fileOffset == r._fileOffset && this->_size == r._size;
	}

	bool operator != (const IndexInfo& r)const { return !(*this == r); }
};

//测试两个类型是否可以转换
template<typename T, typename U>
class Conversion {
private:
	static char Test (U) { }
	static int Test (...) { }
	static T MakeT () { }
public:
	enum { state = (sizeof (Test (MakeT ())) == sizeof (char)) };
};

enum TreeTYPE {
	UNDEF,
	INT,
	FLOAT,
	STRING
};

using namespace std;

