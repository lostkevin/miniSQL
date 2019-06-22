#pragma once
//这是公共定义文件
//有啥宏都可以丢这里
#include <iostream>
#include <vector>
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

//用于where的判断 分别为小于，小于等于，等于，大于等于，大于，不等于
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} WHERE;

//一个struct存放它的一条信息的类型和值
//用一个strunt嵌套一个union实现多种类型的转换
//type的类型：1：int,2:float,1-255:string(数值为字符串的长度+1),注意使用时对Value的选择！
struct Data {
	int type;
	int datai;
	float dataf;
	string datas;
};

//Where存放一组判断语句
struct Where {
	string attr_name;
	Data data; //数据
	WHERE relation_character;   //关系
};

//在确定类型时，慎用str.size()+1来决定str的type的值，一张表最多32个attribute
struct Attribute_s {
	int num;  //存放table的属性数
	std::string name[32];  //存放每个属性的名字
	short type[32];  //存放每个属性的类型，-1：int,0:float,1~255:string的长度+1
	bool unique[32];  //判断每个属性是否unique，是为true
	short primary_key;  //判断是否存在主键,-1为不存在，其他则为主键的所在位置
	bool has_index[32]; //判断是否存在索引
};

//索引管理，一张表最多只能有10个index
struct Index_s {
	int num;  //index的总数
	short location[10];  //每个index在Attribute的name数组中是第几个
	std::string indexname[10];  //每个index的名字
};

using namespace std;

