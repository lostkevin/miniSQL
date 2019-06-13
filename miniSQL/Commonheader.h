#pragma once
//���ǹ��������ļ�
//��ɶ�궼���Զ�����
#include <string>
using namespace std;
//�������Ŀ
#define MAX_BLOCK_NUM 0x1000
//������С
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 0x2000
#endif // !MAX_BLOCK_SIZE
//�㻺������С4K*8K=32M��ʵ�ʻ�������
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * MAX_BLOCK_SIZE)

#ifndef BYTE
#define BYTE char
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
	friend class BufferManager;
	IndexInfo (uint size, uint fileOffset) {
		_fileOffset = fileOffset;
		_size = size;
	}

	IndexInfo (uint fileOffset) {
		_fileOffset = fileOffset;
	}
public:
	friend istream & operator>>(istream & is, IndexInfo & info);
	IndexInfo () {
		_fileOffset = 0;
		_size = MAX_BLOCK_SIZE;
	}
};

using namespace std;

