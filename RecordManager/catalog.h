#ifndef CATALOG_H
#define CATALOG_H
#endif
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <stdlib.h>

#include "..\miniSQL\Commonheader.h"
#include "..\BufferManager\BufferManager.h"

#define BlockNum  10
#define BlockSize 4096
#define Asize 32
#define INT 1
#define CHAR 2
#define FLOAT 3


using namespace std;


//class RowInfo
//{
//public:
//	bool isError;
//	std::string rowName;
//	enum Type { Int, Char, Float } type;
//	int charSize;
//	bool isUnique;
//	bool isPrimary;
//
//	void Analyse(std::string str);
//};
//

struct block {
	string filename;//initial the filename=="",means the block is empty
	char *record;
	bool lock;//if lock==1 this block can't be change
	bool changed;//if changed ==1,this block has been changed
	int offset;
};

int Block_num (string file_name);

class Attribute {		//表格中属性的信息
public:
	string attr_name;	//属性名
	int attr_type;		//属性类（int char float
	int attr_length;
	bool primary;
	bool unique;

};

class Table {		//Table类存储表格信息
public:
	string tablename;
	Attribute attr[Asize];
	string primary_key;
	//	vector <TableInfo> element;

	int attr_num;	//属性数
	bool isError;
	//enum Type { Create, Drop } type;
	//enum Type { Int, Char, Float } type;
	int charSize;

	bool isUnique;
	bool isPrimary;

	void CreateTable (Table info);
	void DropTable (Table info);

	Table () {
		//	tablename="";
	}
	Table (const Table &A)
	{
		tablename = A.tablename;
		for (int i = 0; i < Asize; i++)
			attr[i] = A.attr[i];
		primary_key = A.primary_key;
		attr_num = A.attr_num;
	}
	//Table(TableInfo info) {
	//	tablename = info.tableName;
	//	for (int i = 0; i < info.element.size(); i++) {
	//		attr[i].attr_name = info.element[i].rowName;
	//		if (info.element[i].type == 0) {//INT
	//			attr[i].attr_type = INT;
	//			attr[i].attr_length = 4;
	//		}
	//		else if (info.element[i].type == 1) {//CHAR
	//			attr[i].attr_type = CHAR;
	//			attr[i].attr_length = info.element[i].charSize;
	//		}
	//		else { //FLOAT
	//			attr[i].attr_type = FLOAT;
	//			attr[i].attr_length = 4;
	//		}
	//		attr[i].primary = info.element[i].isPrimary;
	//		if (attr[i].primary == 1)
	//			primary_key = attr[i].attr_name;

	//		attr[i].unique = info.element[i].isUnique;

	//	}

	//	attr_num = info.element.size();
	//}


	//Table(const Table &table) {
	//	tablename = table.tablename;
	//	attr_num = table.attr_num;
	//	for (int i = 0; i < attr_num; i++) {
	//		attr[i] = table.attr[i];
	//	}
	//}


	int getattr_num () {
		int i = 0;
		for (i = 0; i < 32; i++) {
			if (attr[i].attr_name == "")
				break;
		}
		return i;

	}

	int getlength () {
		int len = 0;
		for (int i = 0; i < attr_num; i++) {
			len += attr[i].attr_length;

		}
		return len;
	}

	int primarypos () {
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
	int index_type;
	vector <Attribute> element;
	Attribute attribute;
	index () {
	}/*
	index(IndexInfo indexInfo) {
		index_name = indexInfo.indexName;
		table_name = indexInfo.tableName;

	}*/


};


bool CheckAttrExist (string tablename, string attrname);

bool CheckTableExist (string newtable_name);

Error cCreateTable (Table info);

Error DropTable (Table info);

bool CheckIndexExist (string newindexname);

Error Createindex (IndexInfo indexInfo);

Error Dropindex (IndexInfo indexInfo);

Table readtableinfo (string table_name);

index readindexinfo (string index_name);

