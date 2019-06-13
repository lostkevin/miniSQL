#pragma once
//���ǹ��������ļ�
//��ɶ�궼���Զ�����
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
	//���С
	uint _size;
	//�ļ�ƫ����
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

//�������Ŀ
#define MAX_BLOCK_NUM 4096
//������С
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 8192
#endif // !MAX_BLOCK_SIZE
//�㻺������С4K*8K=32M��ʵ�ʻ�������
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * MAX_BLOCK_SIZE)
