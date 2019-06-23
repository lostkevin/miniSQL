#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "..\miniSQL\Commonheader.h"
#include "..\interpreter\exception.h"

#define Asize 32
using namespace std;

struct Error {
	bool isError;
	string info;
};

struct Attribute {		//表格中属性的信息
	string attr_name;	//属性名
	//属性类型（defined as below: 0 int 1 float 2~256 (char(1) to char(255)
	int attr_type;		
	//在tuple中的位置
	uint offset;
	bool primary;
	bool unique;
};

struct index {
	string index_name;
	string index_file;
	//表示是第几个attribute作为key
	uint keyID;
	//索引类型（defined as below: 0 int 1 float 2~256 (char(1) to char(255)
	int index_type;
};

//Table类存储表信息
//无指针，使用浅拷贝
class Table {		
	friend class CatalogManager;
	//读取特定索引的信息
	index* getIndexinfo (string index_name);
	//删除所有索引文件及数据文件
	void release ();
	//返回主键的信息
	Attribute &getPrimaryKeyInfo ();
public:
	//表名
	string tablename;
	//表对应的文件
	string table_fileName;
	//tuple属性
	Attribute attr[Asize];
	//属性数
	uint attr_num;	
	//索引数组,保存该表所有index信息
	vector<index> IndexBasic;
	Table () {
	}
	//从字符串中读取table信息
	Table (string TableData);
	//将table信息储存到字符串中
	string ToString ();
	bool IsAttrExist (string attr_name);
};

class CatalogManager {
	map<string, Table> DatabaseInfo;
	//不知道有什么用，检查某属性是否存在

	Table* getTableinfo (string table_name);
public:
	//读取整个table的attribute信息
	void getAttrInfo (string table_name, vector<Attribute>& attributsInfo);
	//读取table的index信息
	void getIndex (string table_name, vector<index>& index);
	//读取key是否为unique，参数不正确返回false
	bool getUniqueState (string attr_name, string table_name);
	//获取keyID，理论上建立表时的输入顺序即为keyID，但为了防止遗忘，提供一个接口
	int getKeyID (string attr_name, string table_name);
	//在表上创建索引，失败返回错误信息
	void CreateIndex (index index, string table_name);
	//删除特定索引及其文件，失败返回错误信息
	void Dropindex (string index_name);
	//创建新表，失败返回错误信息
	void CreateTable (Table info);
	//删除某表及所有相关文件，失败返回错误信息
	void DropTable (string table_name);
	//构造时从文件中读取数据库信息
	CatalogManager ();
	//析构时把数据库信息保存到硬盘上
	~CatalogManager ();
	bool CheckTableExist (string table_name);
	bool CheckAttrExist (string table_name, string attrname);
	bool CheckIndexExist (string index_name, string table_name);
};

