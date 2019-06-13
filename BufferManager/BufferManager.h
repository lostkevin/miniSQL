#pragma once
#include <string>
#include <map>
#include <vector>
#include "..\IndexManager\IndexManager.h"


//����������֧�ֶ���ļ�
class BufferManager {
private:
	class Page {
	private:
		//Ϊ�˱�֤Page������д��
		string fileName;
		//����ļ�ͷ��ƫ����
		uint blockOffset;
	public:
		//ָ����ָ��
		BYTE* pBlock;
		const uint _blockSize = MAX_BLOCK_SIZE;
		bool IsDirty;
		Page (string fileName);
		//����ʱӦ�������page
		~Page ();
	};

	class Buffer {
	private:
		struct Node {
			Page *page;
			uint L;
			uint R;
			//trueʱ�޷��Ӷ��е��������Ҳ�޷�д��
			bool PIN;
		};
		uint size;
		Node minHeap[MAX_BLOCK_NUM + 1];
		uint pinnedNum;

	public:
		Buffer ();
		~Buffer ();
		//��page���
		bool Enque (Page * page);
		//����¾ɵ�page���Ѳ��ͷŸ�ҳ���������block��pin=true��return false ���� true
		//�������pinned node ���Բ��ҵ�һ��unpinned node��swap����deque
		bool Deque ();
		//����pin״̬��������true���Ըýڵ�����
		void setPinState (uint i, bool Pinned);
		//���ˣ����ʱ��
		void Up (uint i);
		//����
		void Down (uint i);
	};
	//���С��BLOCK_SIZE�����������ݿ���뵽2^k,�����BLOCK_SIZEʱ���쳣
	//���Ҫ��һ��tuple��size����Ϊ8192 ��256*32������˻���ҳ����8K
	uint ROUND (uint blockSize);
	//������
	Buffer buffer;
	uint blockUsed;
	const uint maxBlock = MAX_BLOCK_NUM;
	//��disk�ж�ȡһ��page��buffer�У��Զ����汻�滻��page, ��ȡ�ɹ�����true
	bool Load (const string &fileName, IndexInfo info);
public:
	//��ȡĳ�����ݿ����Ϣ������Ϣд��resultָ��ָ����ڴ�
	void readRawData (const string& fileName, const IndexInfo &info, BYTE * result);
	//��indexinfoָ���λ��д��size�ֽ�, ����ԴΪpData�������ݲ���buffer�У���Ҫ����
	void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData);
	//�½�һ���飬sizeΪ�����ݿ�Ĵ�С��Ϊ0ʱʹ���ļ�ͷ���ã�=0ʱ�ļ����������쳣��
	const IndexInfo createBlock (const string & fileName, uint size = 0);
	//ɾ��
	void erase (const IndexInfo &info);
	//drop,ɾ���ļ����ͷŸ��ļ������л����
	void drop (const string & fileName);
};

