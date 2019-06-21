// RecordManager.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "RecordManager.h"

string tostring (Tuple tuple) {

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
string Convert_ftos (float Num)
{
	ostringstream oss;
	oss << Num;
	string str (oss.str ());
	return str;
}


/*
	插入一个tuple的函数，table_name插入的表名，insert_tuple插入的tuple
*/
Error Insert_tuple(std::string table_name,Tuple_s insert_tuple) {
	//std::cout << "My Fault\n";
	Error error;

	Table t;
	//获得该表的属性，应在interpreter中判断是否存在表
	t = readtableinfo(table_name);
	string primarykey = t.primary_key;
	Tuple tuple(t);
	//检查是否有错误
	if (!CheckTableExist(tuple.tablename)) {
		error.isError = true;
		error.info = "Error: Table  '" + tuple.tablename + "' doesn't exist ";
		//printf("Error: Table  \"%s\" doesn't exist \n",tuple.tablename.c_str());
		return error;
	}
	//读取插入的tuple的数据
	std::vector<Data> v = insert_tuple.getData();
	if (t.attr_num != v.size()) {
		error.isError = true;
		error.info = "Error: Column count doesn't match value count";
		//	cout<<"Error: Column count doesn't match value count"<<endl;
		return error;
	}

	BufferManager bmgr;
	string filename = ".\\";
	filename= filename + table_name;
	filename = filename + ".bin";
	cout << filename << endl;
	int i = 0, j = 0, k = 0;
	
	string temp;
	char buffer[110000];
	//vector<index> insert_index;
	//readindexinfo(tuple.tablename, insert_index);
	//通过catalog获取indexfilename
	string indexfilename;

	IndexManager iMgr(indexfilename, bmgr);
	
	/*for (i = 0; i < v.size(); j++) {
		Data d = v[j];
		switch (d.type) {
			case -1: {
				int t = getDataLength(d.datai);
				len += t;
			}; break;
			case 0: {
				float t = getDataLength(d.dataf);
				len += t;
			}; break;
			default: {
				len += d.datas.length();
			};
		}
	}*/
	/*for (i = 0; i < tuple.attr_num; i++) {
		if (v.data_[i].type == 0) {
			sprintf(buffer, "%d", insertInfo.element[i].intValue);
			tuple.attr_values[i] = buffer;
		}
		if (insertInfo.element[i].type == 1) {

			tuple.attr_values[i] = insertInfo.element[i].charValue;
		}
		if (insertInfo.element[i].type == 2) {
			ostringstream ss;
			ss << insertInfo.element[i].floatValue;

			tuple.attr_values[i] = ss.str();

		}
	}*/
	//对tuple每个attribute将其转换为字符串型数据，保存进文件
	IndexInfo tmp = bmgr.createBlock(filename, 400);
	//一条tuple的数据
	BYTE array[400] = "";
	for (i = 0; i < tuple.attr_num; j++) {
		Data d = v[i];
		char attr_value[400];
		string data;
		if (d.type == 1) {
			while (d.datai > 0) {
				data = std::to_string(d.datai);
			}
		}
		if (d.type == 2) {
			data=Convert_ftos(d.dataf);
		}
		else if (d.type == 3) {
			data = d.datas;
		}
		strcpy_s(attr_value, data.c_str());
		strcat_s(array, attr_value);
		//如果是primarykey，直接存index
		if (t.attr[i].attr_name == t.primary_key) {
			if (d.type == 1) {
				iMgr.insert(d.datai, tmp);
			}
			if (d.type == 2) {
				iMgr.insert(d.dataf, tmp);
			}
			if (d.type == 3) {
				iMgr.insert(d.datas, tmp);
			}
		}
		//对返回的索引，逐个更新
		int j;
		/*for (j = 0; j < insert_index.size; j++) {
			if (insert_index[j].attribute.attr_name == t.primary_key) {
				continue;
			}
			//通过catalog获取indexfilename
			string indexfilename;
			//对于非primarykey的
			IndexManager Finder(indexfilename, bmgr);
			if (d.type == 1) {
				iMgr.insert(d.datai, Finder.find((int)3170103618));
			}
			if (d.type == 2) {
				iMgr.insert(d.dataf, Finder.find((int)3170103618));
			}
			if (d.type == 3) {
				iMgr.insert(d.datas, Finder.find((int)3170103618));
			}
		}*/
	}
	cout<< array <<endl;
	//std::cout << "No!\n";
}

