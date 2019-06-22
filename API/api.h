//#ifndef API_H
//#define API_H
//#include "../interpreter/basic.h"
//#include "../catalog/catalog.h"
//#include "../IndexManager/IndexManager.h"
//using namespace std;
////API接口。作为Interpreter层操作和底层Manager连接的接口
////包含程序的所有功能接口
////API只做初步的程序功能判断，具体的异常抛出仍由各底层Manager完成
//class API {
//public:
//	//构造函数
//	API();
//	//析构函数
//	~API();
//	//返回包含所有目标属性满足Where条件的记录的表
//	Table selectRecord(string table_name, vector<string> target_attr, vector<Where> where, char operation);
//	//删除对应条件下的Table内记录(不删除表文件)
//	int deleteRecord(string table_name, string target_attr, Where where);
//	//向对应表内插入一条记录
//	void insertRecord(string table_name, Tuple& tuple);
//	//在数据库中插入一个表的元信息
//	bool createTable(string table_name, Attribute attribute, int primary, Index index);
//	//在数据库中删除一个表的元信息，及表内所有记录(删除表文件)
//	bool dropTable(string table_name);
//	//在数据库中创建索引信息
//	bool createIndex(string table_name, string index_name, string attr_name);
//	//删除对应表的对应属性上的索引
//	bool dropIndex(string table_name, string index_name);
//private:
//	//私有函数，用于多条件查询时的and条件合并
//	Table unionTable(Table &table1, Table &table2, string target_attr, Where where);
//	//私有函数，用于多条件查询时的or条件合并
//	Table joinTable(Table &table1, Table &table2, string target_attr, Where where);
//
//protected:
//	//Table table;
//	//index iindex;
//	//IndexManager index;
//	//BufferManager buffer;
//	RecordManager record;
//	CatalogManager catalog;
//};
//
////用于对vector的sort时排序
//bool sortcmp(const Tuple &tuple1, const Tuple &tuple2);
////用于对vector对合并时对排序
//bool calcmp(const Tuple &tuple1, const Tuple &tuple2);
//bool isSatisfied(Tuple& tuple, int target_attr, Where where);
//#endif // !API_H
