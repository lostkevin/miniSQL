#pragma once
#include <string>
#include <map>
#include "..\miniSQL\Commonheader.h"

//����������֧�ֶ���ļ�
class BufferManager {
private:
	class Page {
	private:
		//Ϊ�˱�֤Page������д��
		string fileName;
		//����ļ�ͷ��ƫ����
		uint blockOffset;
		bool IsDirty;
	public:
		//ָ����ָ��
		BYTE* pBlock;
		static const uint _pageSize = PAGE_SIZE;
		bool setDirty () {
			return this->IsDirty ? false : (this->IsDirty = true);
		}
		bool PIN;
		Page (const string &fileName, uint blockOffset);
		const string& getFileName () {
			return fileName;
		}
		const uint getBlockOffset () {
			return blockOffset;
		}
		//����ʱӦ�������page
		~Page ();
	};

	class Buffer {
	private:
		
		Page* minHeap[MAX_BLOCK_NUM + 1];
		//����
		void Down (uint i);

	public:
		Buffer ();
		~Buffer ();
		//��page���
		bool Enque (Page * page);
		//����¾ɵ�page���Ѳ��ͷŸ�ҳ���������block��pin=true��return nullptr
		//�������pinned node,��������Ѻ���deque
		Page* Deque ();
		//ɾ��������ָ���ļ���Page
		void drop (const string& fileName);
		uint size;
		const uint maxSize = MAX_BLOCK_NUM;
	};
	//���С��BLOCK_SIZE�����������ݿ���뵽2^k,�����BLOCK_SIZEʱ���쳣
	//���Ҫ��һ��tuple��size����Ϊ8192 ��256*32������˻���ҳ����8K
	uint ROUND (uint blockSize);
	//������
	Buffer buffer;
	map<string, map<uint, Page*>> BufferIndex;
	//��disk�ж�ȡһ��page��buffer�У��Զ����汻�滻��page, ��ȡ�ɹ�����true
	bool Load (const string &fileName, const IndexInfo &info);
	//��ȡ��IndexInfo��offset���ڵ�Page
	Page* getPage (const string &fileName, const IndexInfo &info);
	uint getFileBlockSize (const string & fileName);
	//��freelist��ȡ���ڵ�
	uint getFreeNode (const string & fileName);
	//��freelist���������
	void AddFreeNode (const string & fileName, uint offset);
public:
	//��ȡĳ�����ݿ����Ϣ������Ϣд��resultָ��ָ����ڴ�, �����ȡʧ�ܣ�����false
	bool readRawData (const string& fileName, const IndexInfo &info, BYTE * result);
	//��indexinfoָ���λ��д��size�ֽ�, ����ԴΪpData�������ݲ���buffer�У���Ҫ����
	void WriteRawData (const string& fileName, const IndexInfo &info, const BYTE * pData);
	//�½�һ���飬sizeΪ�����ݿ�Ĵ�С��Ϊ0ʱʹ���ļ�ͷ���ã�=0ʱ�ļ����������쳣��
	const IndexInfo createBlock (const string & fileName, uint size = 0);
	//ɾ��ĳ��������ݣ������������뵽freelist��
	//����ļ������ڣ�ʲô������
	void erase (const string & fileName, const IndexInfo &info);
	//drop,ɾ���ļ����ͷŸ��ļ������л����
	void drop (const string & fileName);
	//�ж��ļ��Ƿ����
	bool IsFileExist (const string& fileName);
	void setPageState (const string &fileName, const IndexInfo &info, bool state);
};

