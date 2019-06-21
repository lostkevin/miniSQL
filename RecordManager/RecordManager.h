#pragma once
#include"catalog.h"
#include "../interpreter/basic.h"
#include<sstream>
#include"../BufferManager/BufferManager.h"
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

string tostring(Tuple tuple) {

	string result = "";
	int i = 0;
	for (i = 0; tuple.attr_values[i] != ""; i++) {
		result += tuple.attr_values[i];
		if (tuple.attr_values[i + 1] != "")
			result += ",";
	}

	//	 printf("%d\n",i);


	return result;

}


//c++ 将float 类型转换成string 类型:
string Convert_ftos(float Num)
{
	ostringstream oss;
	oss << Num;
	string str(oss.str());
	return str;
}



Error Insert_tuple(std::string table_name, Tuple_s insert_tuple);