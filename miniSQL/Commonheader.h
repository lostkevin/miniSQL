#pragma once
//���ǹ��������ļ�
//��ɶ�궼���Զ�����
#include <string>
using namespace std;
//�������Ŀ
#define MAX_BLOCK_NUM 0x1000
//������С
//Ϊ�˱������ݵĿ�ҳ���棬������32*char��255��= 8192
//Indexû��������⣬һ��pair����260Bytes������һ���ڵ�����������30-40�ȳ�
#define PAGE_SIZE 0x2000
//�㻺������С4K*8K=32M��ʵ�ʻ�������
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * PAGE_SIZE)
#define BYTE char
#define uint unsigned int


class IndexInfo {
private:
	//���С
	uint _size;
	//�ļ�ƫ����
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

//�������������Ƿ����ת��
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

