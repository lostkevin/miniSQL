
//c++ 将float 类型转换成string 类型:
#include<sstream>
#include<iostream>
#include "RecordManager.h"
using namespace std;
int main()
{
	Tuple_s table_s;
	Data d;
	d.type = 1;
	d.datai = 12;
	table_s.addData(d);
	Table t;
	t.tablename = "test2";
	Attribute a;
	a.attr_name = "id";
	a.attr_type = 1;
	a.attr_length = 10;
	a.primary = true;
	t.attr[0] = a;
	t.attr_num = 1;
	t.primary_key = "id";
	Error e = cCreateTable(t);
	cout << e.info << endl;
	//Table_s &tab = table_s;
	e = Insert_tuple("test", table_s);
	cout << e.info << endl;
}
