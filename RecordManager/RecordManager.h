#pragma once
#include<sstream>
#include "../interpreter/basic.h"
#include "../CatalogManager/catalog.h"
#include"../IndexManager/IndexManager.h"

class Tuple : public Table {
public:
	string attr_values[32];
	int attr_num;
	string table_name;
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




Error Insert_tuple(std::string table_name, Tuple_s insert_tuple);
Error select_tuple (string table_name, vector<std::string> target_name, vector<Where> where_select, vector<bool> Operations);
Error delete_tuple (string table_name, vector<Where> where_select);
char* getword(int offset, int length, char *rawdata);