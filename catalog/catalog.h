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
//缓冲块大小
//为了避免数据的跨页储存，极限是32*char（255）= 8192
//Index没有这个问题，一个pair至多260Bytes，所以一个节点至少能做到30-40扇出
#define PAGE_SIZE 0x2000
//裸缓冲区大小4K*8K=32M，实际会比这更大
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
//	bool delete_value(string filename, int offset);//删除文件中指定位置存储在缓冲区的块
//};

class IndexInfo {
private:
	//块大小
	uint _size;
	//文件偏移量
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
		//为了保证Page能自行写回
		string fileName;
		//相对文件头的偏移量
		uint blockOffset;
		bool IsDirty;
	public:
		//指向块的指针
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
		//析构时应保存这个page
		~Page();
	};

	class Buffer {
	private:

		Page* minHeap[MAX_BLOCK_NUM + 1];
		//下滤
		void Down(uint i);

	public:
		Buffer();
		~Buffer();
		//将page入堆
		bool Enque(Page * page);
		//将最陈旧的page出堆并释放该页，如果所有block都pin=true，return nullptr
		//如果遇到pinned node,重新整理堆后再deque
		Page* Deque();
		//删除来自于指定文件的Page
		void drop(const string& fileName);
		uint size;
		const uint maxSize = MAX_BLOCK_NUM;
	};
	//块大小是BLOCK_SIZE，将所有数据块对齐到2^k,块大于BLOCK_SIZE时抛异常
	//大程要求一个tuple的size至多为8192 （256*32），因此缓冲页必须8K
	static uint ROUND(uint blockSize);
	static uint getFileSize(const string& fileName);
	//缓冲区
	Buffer buffer;
	map<string, map<uint, Page*>> BufferIndex;
	//从disk中读取一个page到buffer中，自动保存被替换的page, 读取成功返回true，文件不存在返回false
	bool Load(const string &fileName, const IndexInfo &info);
	//读取该IndexInfo的offset所在的Page，文件不存在返回nullptr
	Page* getPage(const string &fileName, const IndexInfo &info);
	uint getFileBlockSize(const string & fileName);
	//向freelist中取出节点
	uint getFreeNode(const string & fileName);
	//向freelist中添加数据
	void AddFreeNode(const string & fileName, uint offset);
public:
	BufferManager() {

	}
	//读取某个数据块的信息，将信息写到result指针指向的内存, 如果读取失败，返回false
	bool readRawData(const string& fileName, const IndexInfo &info, BYTE * result);
	//向indexinfo指向的位置写入size字节, 数据源为pData,若文件不存在，要求
	void WriteRawData(const string& fileName, const IndexInfo &info, const BYTE * pData);
	//新建一个块，size为该数据块的大小，为0时使用文件头设置（=0时文件不存在抛异常）
	//文件不存在将检查size是否被设置，如果非零将顺便新建一个块大小为该值的文件。
	//若文件名不正确，抛异常
	//此后可以使用getPage
	const IndexInfo createBlock(const string & fileName, uint size = 0);
	//删除某个块的数据，即将这个块加入到freelist内
	//如果文件不存在，什么都不做
	void erase(const string & fileName, const IndexInfo &info);
	//drop,删除文件，释放该文件的所有缓冲块
	void drop(const string & fileName);
	//判断文件是否存在
	bool IsFileExist(const string& fileName);
	//将某Page加载到buffer中并使他的Pin=给定值
	void setPageState(const string &fileName, const IndexInfo &info, bool state);
};

int Block_num(string file_name);


class Attribute {		//表格中属性的信息
public:
	string attr_name;
	int attr_type;
	int attr_length;
	bool primary;
	bool unique;

};

class Table {		//Table类存储表格信息
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

Error Insert(InsertInfo insertInfo);  //insert记录 返回出错信息 

selectError  Select_Delete(SDInfo sdInfo); //select & delete 返回出错信息 

string  tostring(Tuple tuple);

Tuple searchrecord(int offset, string table_name);  //给定offset 与 tablename 返回一个tuple 

