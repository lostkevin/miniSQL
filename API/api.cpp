#include "api.h"
//#include "template_function.h"
#include <algorithm>
#include <vector>
#include <iterator>
using namespace std;
//构造函数
API::API() {}

//析构函数
API::~API() {}
//返回包含所有目标属性满足Where条件的记录的表
Table API::selectRecord(string table_name, vector<string> target_attr, vector<Where> where, char operation)
{
	//输入表名，条件
	if (target_attr.size() == 0) {
		return record.selectRecord(table_name);
	}
	else if (1 == target_attr.size()) {
		return record.selectRecord(table_name, target_attr[0], where[0]);
	}
	else {
		Table table1 = record.selectRecord(table_name, target_attr[0], where[0]);
		Table table2 = record.selectRecord(table_name, target_attr[1], where[1]);

		if (operation)
			return joinTable(table1, table2, target_attr[0], where[0]);
		else
			return unionTable(table1, table2, target_attr[0], where[0]);
	}
}
//删除对应条件下的Table内记录(不删除表文件)
int API::deleteRecord(string table_name, string target_attr, Where where)
{
	int result;
	if (target_attr == "")
		result = record.deleteRecord(table_name);
	else
		result = record.deleteRecord(table_name, target_attr, where);
	return result;
}

//向对应表内插入一条记录
void API::insertRecord(string table_name, Tuple& tuple)
{
	record.insertRecord(table_name, tuple);
	return;
}
//在数据库中插入一个表的元信息
bool API::createTable(string table_name, Attribute attribute, int primary, Index index)
{
	record.createTableFile(table_name);
	catalog.createTable(table_name, attribute, primary, index);

	return true;
}
//在数据库中删除一个表的元信息，及表内所有记录(删除表文件)
bool API::dropTable(string table_name)
{
	record.dropTableFile(table_name);
	catalog.dropTable(table_name);
	return true;
}
//功能：在数据库中创建新的索引信息
bool API::createIndex(string table_name, string index_name, string attr_name)
{
	IndexManager index(table_name);

	std::string file_path = "INDEX_FILE_" + attr_name + "_" + table_name;
	string file_path = "INDEX_FILE_" + attr_name + "_" + table_name;
	int type;
	catalog.createIndex(table_name, attr_name, index_name);
	Attribute attr = catalog.getAttribute(table_name);
	for (int i = 0; i < attr.num; i++) {
		if (attr.name[i] == attr_name) {
			type = (int)attr.type[i];
			break;
		}
	}
	index.open(file_path);
	record.createIndex(table_name, index, attr_name);

	return true;
}
//删除对应表的对应属性上的索引
bool API::dropIndex(std::string table_name, std::string index_name)
{
	IndexManager index(table_name);

	string attr_name = catalog.IndextoAttr(table_name, index_name);
	string file_path = "INDEX_FILE_" + attr_name + "_" + table_name;
	int type;

	Attribute attr = catalog.getAttribute(table_name);
	for (int i = 0; i < attr.num; i++) {
		if (attr.name[i] == attr_name) {
			type = (int)attr.type[i];
			break;
		}
	}
	index.dropIndex();
	catalog.dropIndex(table_name, index_name);

	file_path = "./database/index/" + file_path;
	remove(file_path.c_str());
	return true;
}

//私有函数，用于多条件查询时的or条件合并
Table API::unionTable(Table &table1, Table &table2, std::string target_attr, Where where)
{
	Table result_table(table1);
	vector<Tuple>& result_tuple = result_table.getTuple();
	vector<Tuple> tuple1 = table1.getTuple();
	vector<Tuple> tuple2 = table2.getTuple();
	result_tuple = tuple1;

	//std::vector<Tuple>().swap(result_tuple);

	int i;
	Attribute attr = table1.getAttr();
	for (i = 0; i < 32; i++)
		if (attr.name[i] == target_attr)
			break;

	for (int j = 0; j < tuple2.size(); j++)
		if (!isSatisfied(tuple2[j], i, where))
			result_tuple.push_back(tuple2[j]);

	sort(result_tuple.begin(), result_tuple.end(), sortcmp);
	return result_table;
	
}

//私有函数，用于多条件查询时的and条件合并
Table API::joinTable(Table &table1, Table &table2, string target_attr, Where where)
{
	Table result_table(table1);
	vector<Tuple>& result_tuple = result_table.getTuple();
	vector<Tuple> tuple1 = table1.getTuple();
	vector<Tuple> tuple2 = table2.getTuple();

	int i;
	Attribute attr = table1.getAttr();
	for (i = 0; i < 32; i++)
		if (attr.name[i] == target_attr)
			break;

	for (int j = 0; j < tuple2.size(); j++)
		if (isSatisfied(tuple2[j], i, where))
			result_tuple.push_back(tuple2[j]);

	std::sort(result_tuple.begin(), result_tuple.end(), sortcmp);
	return result_table;

}

//用于对vector的sort时排序
bool sortcmp(const Tuple &tuple1, const Tuple &tuple2)
{
	vector<Data> data1 = tuple1.getData();
	vector<Data> data2 = tuple2.getData();

	switch (data1[0].type) {
	case -1: return data1[0].datai < data2[0].datai;
	case 0: return data1[0].dataf < data2[0].dataf;
	default: return data1[0].datas < data2[0].datas;
	}
}

//用于对vector对合并时排序
bool calcmp(const Tuple &tuple1, const Tuple &tuple2)
{
	int i;

	vector<Data> data1 = tuple1.getData();
	vector<Data> data2 = tuple2.getData();

	for (i = 0; i < data1.size(); i++) {
		bool flag = false;
		switch (data1[0].type) {
		case -1: {
			if (data1[0].datai != data2[0].datai)
				flag = true;
		}break;
		case 0: {
			if (data1[0].dataf != data2[0].dataf)
				flag = true;
		}break;
		default: {
			if (data1[0].datas != data2[0].datas)
				flag = true;
		}break;
		}
		if (flag)
			break;
	}


	if (i == data1.size())
		return true;
	else
		return false;
}

bool isSatisfied(Tuple& tuple, int target_attr, Where where)
{
	vector<Data> data = tuple.getData();

	switch (where.relation_character) {
	case LESS: {
		switch (where.data.type) {
		case -1: return (data[target_attr].datai < where.data.datai); break;
		case 0: return (data[target_attr].dataf < where.data.dataf); break;
		default: return (data[target_attr].datas < where.data.datas); break;
		}
	} break;
	case LESS_OR_EQUAL: {
		switch (where.data.type) {
		case -1: return (data[target_attr].datai <= where.data.datai); break;
		case 0: return (data[target_attr].dataf <= where.data.dataf); break;
		default: return (data[target_attr].datas <= where.data.datas); break;
		}
	} break;
	case EQUAL: {
		switch (where.data.type) {
		case -1: return (data[target_attr].datai == where.data.datai); break;
		case 0: return (data[target_attr].dataf == where.data.dataf); break;
		default: return (data[target_attr].datas == where.data.datas); break;
		}
	} break;
	case GREATER_OR_EQUAL: {
		switch (where.data.type) {
		case -1: return (data[target_attr].datai >= where.data.datai); break;
		case 0: return (data[target_attr].dataf >= where.data.dataf); break;
		default: return (data[target_attr].datas >= where.data.datas); break;
		}
	} break;
	case GREATER: {
		switch (where.data.type) {
		case -1: return (data[target_attr].datai > where.data.datai); break;
		case 0: return (data[target_attr].dataf > where.data.dataf); break;
		default: return (data[target_attr].datas > where.data.datas); break;
		}
	} break;
	case NOT_EQUAL: {
		switch (where.data.type) {
		case -1: return (data[target_attr].datai != where.data.datai); break;
		case 0: return (data[target_attr].dataf != where.data.dataf); break;
		default: return (data[target_attr].datas != where.data.datas); break;
		}
	} break;
	default:break;
	}

	return false;
}