#pragma once
#include <string>
#include "stdlib.h"
#include <vector>
#include "..\IndexManager\IndexManager.h"
using namespace std;

//�������Ŀ
#define MAX_BLOCK_NUM 4096
//������С
#define MAX_BLOCK_SIZE 8192
//��������С4K*8K=32M
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * MAX_BLOCK_SIZE)

#ifndef BYTE
#define BYTE signed char
#endif

//table�����ʽ
//0x00 ����freelist������������
//֮���ƫ��������2^k����
//������Ŀռ�������������BLOCK_SIZE
//����������Ϊ~50%
//ɾ��ʱ������������ļ���������޸�������freelist��
//һ��tableһ���ļ������ڴ���

class Buffer {
private:
	string fileName;
	BYTE* pBlock;
	uint tupleSize;
	bool PIN;
	//����ļ�ͷ��ƫ����
	uint blockOffset;
	//���С��BLOCK_SIZE��������tuple���뵽2^k,tuple����BLOCK_SIZEʱ���쳣
	//���Ҫ��һ��tuple��size����Ϊ8160 ��255*32��
	uint ROUND ();
public:
	const uint _blockSize = MAX_BLOCK_SIZE;
	bool IsDirty;
	Buffer ();
	Buffer (const string &fileName);
	~Buffer ();
	
};

//����������֧�ֲ�ͬtable��ͬʱ��д�����buffer�ڲ�����tuple��size
class BufferManager {
private:
	//������
	Buffer* BufferRegion;
	uint blockUsed;
	const uint maxBlock = MAX_BLOCK_NUM;
	IndexManager indexManager;
	//��������block��disk��,������ֵ��ʧ�����쳣
	void SaveAll ();
	//����,���ظ�tuple��handle
	template<typename _KTy>BYTE* getTuple (const _KTy & key);
	//��Χ����������tuple��handle
	template<typename _KTy>vector<BYTE*> getTuple (const _KTy & min, const _KTy & max);
	//��disk�ж�ȡһ��block��buffer�У��Զ����汻�滻��buffer, ��ȡ�ɹ�����true
	//��Ҫ����һ��tuple��size
	bool Load (const string &fileName, uint tupleSize);
	//����һ����tuple�����ļ��������ڣ��½�
	const IndexInfo newTuple (const string &fileName);
public:
	//create tuple, �½�һ��tuple����pDataָ��ָ���ֵ���Ƶ����tuple��
	//����ļ������ڣ��½��ļ�
	//�ļ�������freelist�ǿգ���freelistָ���λ�ö�Ӧ��block
	//�ļ�������freelistΪ�գ����½�buffer��bufferָ���ļ�β��
	//���ظ�tuple������
	const IndexInfo createTuple (const string & fileName, const BYTE*result, const uint size);
	//��ȡĳ��tuple����Ϣ������Ϣд��resultָ��ָ����ڴ�
	void readData (const IndexInfo &info, BYTE * result, const uint size);
	//�޸�tuple��Ϊ�˷���ʵ�֣�ʹ���ֽ�ֱ��д��,��PData��bufferָ��λ��д��size bytes
	void Write (const IndexInfo &info, const BYTE* pData, uint size);
	//ɾ�������ÿ����freelist�������0x00��ʹ�ɾ���Ŀ�
	void eraseTuple (const IndexInfo &info);
	//drop,ɾ������table������
	void dropTable (const string & fileName);
};

