#pragma once
#include "..\miniSQL\Commonheader.h"

//元组管理，数据只增不减
class Tuple_s {
private:
	std::vector<Data> data_;  //存储元组里的每个数据的信息
	bool isDeleted_;
public:
	Tuple_s() : isDeleted_(false) {};
	Tuple_s(const Tuple_s &tuple_in);  //拷贝元组
	void addData(Data data_in);  //新增元组
	std::vector<Data> getData() const;  //返回数据
	int getSize();  //返回元组的数据数量
	bool isDeleted();
	void setDeleted();
	void showTuple();  //显示元组中的所有数据
};

class Table_s {
private:
	std::string title_;  //表名
	std::vector<Tuple_s> tuple_;  //存放所有的元组
	Index_s index_;  //表的索引信息
public:
	Attribute_s attr_;  //数据的类型
	//构造函数
	Table_s() {};
	Table_s(std::string title, Attribute_s attr);
	Table_s(const Table_s &table_in);

	// int DataSize();  //每个tuple占的数据大小

	int setIndex(short index, std::string index_name);  //插入索引，输入要建立索引的Attribute的编号，以及索引的名字，成功返回1失败返回0
	int dropIndex(std::string index_name);  //删除索引，输入建立的索引的名字，成功返回1失败返回0

	//private的输出接口
	std::string getTitle();
	Attribute_s getAttr();
	std::vector<Tuple_s>& getTuple();
	Index_s getIndex();
	short gethasKey();

	void showTable(); //显示table的部分数据
	void showTable(int limit);
};
