#ifndef CATALOG_H
#define CATALOG_H
#endif
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <map>

#define BlockNum  10
#define BlockSize 4096
#define Asize 32
#define INT 1
#define CHAR 2
#define FLOAT 3
#define MAX_BLOCK_NUM 0x1000
//������С
//Ϊ�˱������ݵĿ�ҳ���棬������32*char��255��= 8192
//Indexû��������⣬һ��pair����260Bytes������һ���ڵ�����������30-40�ȳ�
#define PAGE_SIZE 0x2000
//�㻺������С4K*8K=32M��ʵ�ʻ�������
#define MAX_BUFFER_SIZE (MAX_BLOCK_NUM * PAGE_SIZE)
#define BYTE char
#define uint unsigned int

using namespace std;


class StringM
{
public:
	bool isCal(char ch);
	bool NameSyntax(std::string name);
	std::string DeleteBlank(std::string input);
	std::pair<bool, int> String2Num(std::string input);
	std::pair<std::string, std::string> GetNextWord(std::string input);
	std::pair<std::string, std::string> GetNextComma(std::string input);
	std::pair<std::string, std::string> GetNextAnd(std::string input);
	std::pair<std::string, std::string> GetNextCal(std::string input);
	std::pair<bool, std::pair<std::string, std::string> > GetNextBracket(std::string str);
	std::string Int2String(int n);
};


extern StringM stringM;


class ReadString
{
public:
	enum Type { SQL, Quit, Exec, Unkown, Other } type;
	std::string str;
	void ReadLine();
	void ReadFromString(std::string str);
};


class SQLStatement
{
public:
	bool isError;
	enum Type {
		Create_table,
		Create_index,
		Drop_table,
		Drop_index,
		Select,
		Insert,
		Delete
	} type;
	std::string str;
	void Analyse(ReadString readString);
};

class RowInfo
{
public:
	bool isError;
	std::string rowName;
	enum Type { Int, Char, Float } type;
	int charSize;
	bool isUnique;
	bool isPrimary;

	void Analyse(std::string str);
};

class TableInfo
{
public:
	bool isError;
	enum Type { Create, Drop } type;
	std::string tableName;
	std::vector<RowInfo> element;

	void CreateTable(SQLStatement sql);
	void DropTable(SQLStatement sql);
	std::pair<bool, std::string> AnalysePrimary(std::string str);
};

class Index_Info
{
public:
	bool isError;
	enum Type { Create, Drop } type;
	std::string indexName;
	std::string tableName;
	std::vector<RowInfo> element;

	void CreateIndex(SQLStatement sql);
	void DropIndex(SQLStatement sql);
};

class ValueInfo
{
public:
	bool isError;
	enum Type { Int, Char, Float } type;
	int intValue;
	float floatValue;
	std::string charValue;

	void Analyse(std::string str);
};

class InsertInfo
{
public:
	bool isError;
	std::string tableName;
	std::vector<ValueInfo> element;

	void Analyse(SQLStatement sql);
};

class CondInfo
{
public:
	bool isError;
	ValueInfo cons;
	std::string rowName;
	enum Type {
		Equal,
		nEqual,
		Less,
		Greater,
		nGreater,
		nLess
	} type;

	void Analyse(std::string str);
};

class SDInfo
{
public:
	bool isError;
	enum Type { Select, Delete } type;
	std::string tableName;
	int a;
	bool noCond;
	std::vector<string> attribute;
	std::vector<CondInfo> element;

	void Analyse(SQLStatement sql);
};

struct block {
	string filename;//initial the filename=="",means the block is empty
	char *record;
	bool lock;//if lock==1 this block can't be change
	bool changed;//if changed ==1,this block has been changed
	int offset;
};

//class buffermanager {
//public:
//	block* buffer[10000];
//	buffermanager();
//	~buffermanager();
//	block* get_block();
//	void delete_block(int i);
//	void initial_buffer();
//	bool write_allbufferTofile();
//	block*findblock(string filename, int offset, bool lock);//filename and offset must be satisfied at the same time 
//	bool write_blockTofile(string filename, int offset);
//	bool isfull();
//	bool write_fileToblock(string filename, int offset, bool lock);
//	bool delete_value(string filename, int offset);//ɾ���ļ���ָ��λ�ô洢�ڻ������Ŀ�
//};

class IndexInfo {
private:
	//���С
	uint _size;
	//�ļ�ƫ����
	uint _fileOffset;
	//friend class IndexManager;
	friend class BufferManager;
	friend int main();
	template <typename _Ty>friend class Index;
	IndexInfo(uint size, uint fileOffset) {
		_fileOffset = fileOffset;
		_size = size;
	}

	IndexInfo(uint fileOffset) {
		_fileOffset = fileOffset;
	}
public:
	IndexInfo() {
		_fileOffset = 0;
		_size = PAGE_SIZE;
	}
	bool operator < (const IndexInfo& r) const {
		return this->_fileOffset < r._fileOffset;
	}

	bool operator == (const IndexInfo& r)const {
		return this->_fileOffset == r._fileOffset && this->_size == r._size;
	}

	bool operator != (const IndexInfo& r)const { return !(*this == r); }
};

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
		bool setDirty() {
			return this->IsDirty ? false : (this->IsDirty = true);
		}
		bool PIN;
		Page(const string &fileName, uint blockOffset);
		const string& getFileName() {
			return fileName;
		}
		const uint getBlockOffset() {
			return blockOffset;
		}
		//����ʱӦ�������page
		~Page();
	};

	class Buffer {
	private:

		Page* minHeap[MAX_BLOCK_NUM + 1];
		//����
		void Down(uint i);

	public:
		Buffer();
		~Buffer();
		//��page���
		bool Enque(Page * page);
		//����¾ɵ�page���Ѳ��ͷŸ�ҳ���������block��pin=true��return nullptr
		//�������pinned node,��������Ѻ���deque
		Page* Deque();
		//ɾ��������ָ���ļ���Page
		void drop(const string& fileName);
		uint size;
		const uint maxSize = MAX_BLOCK_NUM;
	};
	//���С��BLOCK_SIZE�����������ݿ���뵽2^k,�����BLOCK_SIZEʱ���쳣
	//���Ҫ��һ��tuple��size����Ϊ8192 ��256*32������˻���ҳ����8K
	static uint ROUND(uint blockSize);
	static uint getFileSize(const string& fileName);
	//������
	Buffer buffer;
	map<string, map<uint, Page*>> BufferIndex;
	//��disk�ж�ȡһ��page��buffer�У��Զ����汻�滻��page, ��ȡ�ɹ�����true���ļ������ڷ���false
	bool Load(const string &fileName, const IndexInfo &info);
	//��ȡ��IndexInfo��offset���ڵ�Page���ļ������ڷ���nullptr
	Page* getPage(const string &fileName, const IndexInfo &info);
	uint getFileBlockSize(const string & fileName);
	//��freelist��ȡ���ڵ�
	uint getFreeNode(const string & fileName);
	//��freelist���������
	void AddFreeNode(const string & fileName, uint offset);
public:
	BufferManager() {

	}
	//��ȡĳ�����ݿ����Ϣ������Ϣд��resultָ��ָ����ڴ�, �����ȡʧ�ܣ�����false
	bool readRawData(const string& fileName, const IndexInfo &info, BYTE * result);
	//��indexinfoָ���λ��д��size�ֽ�, ����ԴΪpData,���ļ������ڣ�Ҫ��
	void WriteRawData(const string& fileName, const IndexInfo &info, const BYTE * pData);
	//�½�һ���飬sizeΪ�����ݿ�Ĵ�С��Ϊ0ʱʹ���ļ�ͷ���ã�=0ʱ�ļ����������쳣��
	//�ļ������ڽ����size�Ƿ����ã�������㽫˳���½�һ�����СΪ��ֵ���ļ���
	//���ļ�������ȷ�����쳣
	//�˺����ʹ��getPage
	const IndexInfo createBlock(const string & fileName, uint size = 0);
	//ɾ��ĳ��������ݣ������������뵽freelist��
	//����ļ������ڣ�ʲô������
	void erase(const string & fileName, const IndexInfo &info);
	//drop,ɾ���ļ����ͷŸ��ļ������л����
	void drop(const string & fileName);
	//�ж��ļ��Ƿ����
	bool IsFileExist(const string& fileName);
	//��ĳPage���ص�buffer�в�ʹ����Pin=����ֵ
	void setPageState(const string &fileName, const IndexInfo &info, bool state);
};

int Block_num(string file_name);


class Attribute {		//��������Ե���Ϣ
public:
	string attr_name;
	int attr_type;
	int attr_length;
	bool primary;
	bool unique;

};

class Table {		//Table��洢�����Ϣ
public:
	string tablename;
	Attribute attr[Asize];
	string primary_key;
	int attr_num;
	Table() {
		//	tablename="";
	}
	Table(const Table &A)
	{
		tablename = A.tablename;
		for (int i = 0; i < Asize; i++)
			attr[i] = A.attr[i];
		primary_key = A.primary_key;
		attr_num = A.attr_num;
	}
	Table(TableInfo info) {
		tablename = info.tableName;
		for (int i = 0; i < info.element.size(); i++) {
			attr[i].attr_name = info.element[i].rowName;
			if (info.element[i].type == 0) {//INT
				attr[i].attr_type = INT;
				attr[i].attr_length = 4;
			}
			else if (info.element[i].type == 1) {//CHAR
				attr[i].attr_type = CHAR;
				attr[i].attr_length = info.element[i].charSize;
			}
			else { //FLOAT
				attr[i].attr_type = FLOAT;
				attr[i].attr_length = 4;
			}
			attr[i].primary = info.element[i].isPrimary;
			if (attr[i].primary == 1)
				primary_key = attr[i].attr_name;

			attr[i].unique = info.element[i].isUnique;

		}

		attr_num = info.element.size();
	}

	Table(Table &table) {
		tablename = table.tablename;
		attr_num = table.attr_num;
		for (int i = 0; i < attr_num; i++) {
			attr[i] = table.attr[i];
		}
	}


	int getattr_num() {
		int i = 0;
		for (i = 0; i < 32; i++) {
			if (attr[i].attr_name == "")
				break;
		}
		return i;

	}

	int getlength() {
		int len = 0;
		for (int i = 0; i < attr_num; i++) {
			len += attr[i].attr_length;

		}
		return len;
	}

	int primarypos() {
		int i = 0;
		for (i = 0; i < 32; i++) {
			if (attr[i].primary == true) {
				break;
			}
		}
		return i;
	}
};

class Error {
public:
	bool isError;
	string info;
};


class index {
public:
	string index_name;
	string table_name;
	// string attr_name;
	vector <Attribute> element;
	index() {
	}
	index(Index_Info indexInfo) {
		index_name = indexInfo.indexName;
		table_name = indexInfo.tableName;
		Attribute a;
		for (int i = 0; i < indexInfo.element.size(); i++) {
			a.attr_name = indexInfo.element[i].rowName;
			element.push_back(a);
		}
	}
};

class Tuple : public Table {
public:
	string attr_values[32];
	Tuple() {
		//	tablename="";
	}
	Tuple(Table &table) :Table(table) {
	}
};

class selectvalue {
public:
	string attr_name[32];
	string attr_values[32];
	int count;
};
class selectError {

public:
	bool isError;
	string info;
	int count;
	vector <selectvalue> element;

};

bool CheckAttrExist(string tablename, string attrname);

bool CheckTableExist(string newtable_name);

Error cCreateTable(TableInfo info);

Error DropTable(TableInfo info);

bool CheckIndexExist(string newindexname);

Error Createindex(Index_Info indexInfo);

Error Dropindex(Index_Info indexInfo);

Table readtableinfo(string table_name);

Index_Info readindexinfo(string index_name);

Error Insert(InsertInfo insertInfo);  //insert��¼ ���س�����Ϣ 

selectError  Select_Delete(SDInfo sdInfo); //select & delete ���س�����Ϣ 

string  tostring(Tuple tuple);

Tuple searchrecord(int offset, string table_name);  //����offset �� tablename ����һ��tuple 

