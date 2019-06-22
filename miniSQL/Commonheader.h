#pragma once
//���ǹ��������ļ�
//��ɶ�궼���Զ�����
#include <iostream>
#include <vector>
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

//����where���ж� �ֱ�ΪС�ڣ�С�ڵ��ڣ����ڣ����ڵ��ڣ����ڣ�������
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} WHERE;

//һ��struct�������һ����Ϣ�����ͺ�ֵ
//��һ��struntǶ��һ��unionʵ�ֶ������͵�ת��
//type�����ͣ�1��int,2:float,1-255:string(��ֵΪ�ַ����ĳ���+1),ע��ʹ��ʱ��Value��ѡ��
struct Data {
	int type;
	int datai;
	float dataf;
	string datas;
};

//Where���һ���ж����
struct Where {
	string attr_name;
	Data data; //����
	WHERE relation_character;   //��ϵ
};

//��ȷ������ʱ������str.size()+1������str��type��ֵ��һ�ű����32��attribute
struct Attribute_s {
	int num;  //���table��������
	std::string name[32];  //���ÿ�����Ե�����
	short type[32];  //���ÿ�����Ե����ͣ�-1��int,0:float,1~255:string�ĳ���+1
	bool unique[32];  //�ж�ÿ�������Ƿ�unique����Ϊtrue
	short primary_key;  //�ж��Ƿ��������,-1Ϊ�����ڣ�������Ϊ����������λ��
	bool has_index[32]; //�ж��Ƿ��������
};

//��������һ�ű����ֻ����10��index
struct Index_s {
	int num;  //index������
	short location[10];  //ÿ��index��Attribute��name�������ǵڼ���
	std::string indexname[10];  //ÿ��index������
};

using namespace std;

