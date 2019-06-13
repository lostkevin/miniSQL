#pragma once
#include <string>
#include <map>
#include <vector>
#include "..\IndexManager\IndexManager.h"
using namespace std;

//�������Ŀ
#define MAX_BLOCK_NUM 4096
//������С
#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 8192
#endif // !MAX_BLOCK_SIZE
//��������С4K*8K=32M
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * MAX_BLOCK_SIZE)

#ifndef BYTE
#define BYTE signed char
#endif

//table�����ʽ
//0x00 ����freelist������������
//֮���ƫ��������2^k����
//������Ŀռ�������������BLOCK_SIZE
//ɾ��ʱ���޸�������freelist
//һ��tableһ���ļ������ڴ���

//����������֧�ֶ���ļ�
class BufferManager {
private:
	class Page {
	private:
		//Ϊ�˱�֤Page������д��
		string fileName;
		bool PIN; //trueʱ�޷�д�ص��ļ�
		//����ļ�ͷ��ƫ����
		uint blockOffset;
		//���С��BLOCK_SIZE�����������ݿ���뵽2^k,�����BLOCK_SIZEʱ���쳣
		//���Ҫ��һ��tuple��size����Ϊ8192 ��256*32������˻���ҳ����8K
		uint ROUND ();
	public:
		//ָ����ָ��
		BYTE* pBlock;
		const uint _blockSize = MAX_BLOCK_SIZE;
		bool IsDirty;
		Page ();
		//����ʱӦ�������page
		~Page ();
		void setFree ();
	};

	//������
	Page* Buffer;
	uint blockUsed;
	const uint maxBlock = MAX_BLOCK_NUM;
	//��disk�ж�ȡһ��page��buffer�У��Զ����汻�滻��page, ��ȡ�ɹ�����true
	bool Load (const string &fileName, IndexInfo info);
	//����ļ������ڣ��½��ļ�
	//�ļ�������freelist�ǿգ���freelistָ���λ�ö�Ӧ��block
	//�ļ�������freelistΪ�գ����½�buffer��bufferָ���ļ�β��
	//���ظ�Page������
	const IndexInfo createPage (const string & fileName);
public:
	//��ȡĳ�����ݿ����Ϣ������Ϣд��resultָ��ָ����ڴ�
	void readRawData (const string& fileName, const IndexInfo &info, BYTE * result);
	//��indexinfoָ���λ��д��size�ֽ�, ����ԴΪpData�������ݲ���buffer�У���Ҫ����
	void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData);
	//ɾ��
	void erase (const IndexInfo &info);
	//drop,ɾ���ļ����ͷŸ��ļ������л����
	void drop (const string & fileName);
};

