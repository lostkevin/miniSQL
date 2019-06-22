// RecordManager.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "RecordManager.h"
#include <cstring>

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


/*
	插入一个tuple的函数，table_name插入的表名，insert_tuple插入的tuple
	默认inset_tuple补足了值为null的属性，使insert_tuple与attr_info的属性对齐
*/
Error Insert_tuple(std::string table_name, Tuple_s insert_tuple) {
	CatalogManager cmgr;
	Error error;					//返回错误信息
	//Table* table;					//表的信息
	vector<Attribute> attr_info;	//元祖的所有属性
	vector<index> index_info;	 	//表的所有index信息
	vector<Data> insert_att_data = insert_tuple.getData();
	//获得该表的属性，应在interpreter中判断是否存在表
	/*
		获取表的相关信息，包括attribute、index。
		获取插入的tuple的信息，只包含data的type和值
	*/
	cmgr.getAttrInfo(table_name, attr_info);
	cmgr.getIndex(table_name, index_info);
	//检查是否有错误

	BufferManager bmgr;
	string filename = ".\\";
	filename = filename + table_name;
	filename = filename + ".dat";
	cout << filename << endl;
	int i = 0, j = 0, k = 0;

	string temp;
	//获取primarykey的名称，加入到index_name中，方便之后查找indexfilename
	string index_name = "primary_";
	string primarykey_name;
	int primary_valuei;
	float primary_valuef;
	string primary_values;
	for (i = 0; i < attr_info.size(); i++) {
		if (attr_info[i].primary == true) {
			index_name = index_name + attr_info[i].attr_name;
			primarykey_name = attr_info[i].attr_name;
			break;
		}
	}
	//用获得的主键的indexname，在该表的index列表中查找indexfilename
	index primarykey;
	for (i = 0; i < index_info.size(); i++) {
		if (index_name == index_info[i].index_name) {
			primarykey = index_info[i];
		}
	}
	//初始化存主键的indexmanager
	IndexManager iMgr(primarykey.index_file, bmgr);

	//声明一个数组来记录该表所有的indexinfo
	vector<IndexInfo> all_indexinfo;
	iMgr.getAllIndex(all_indexinfo);
	//所有tuple逐个对比
	for (i = 0; i < all_indexinfo.size(); i++) {
		//逐个读取rawdata
		char* rawdata = new BYTE[PAGE_SIZE]{ 0 };
		bmgr.readRawData(filename, all_indexinfo[i], rawdata);
		//读取一个tuple的数据
		vector<Data> att_data;
		int offset = 0;
		for (j = 0; j < attr_info.size(); j++) {
			Data temp;
			char *attr_rawdata = getword(offset, attr_info[j].offset, rawdata);
			temp.type = attr_info[j].attr_type;
			//int
			if (temp.type == 1) {
				temp.datai = atoi(attr_rawdata);
			}
			//float
			else if (temp.type == 2) {
				temp.dataf = atof(attr_rawdata);
			}
			//char
			else {
				temp.datas = attr_rawdata;
			}
			free (attr_rawdata);
			att_data.push_back(temp);
		}
		//将该tuple与插入的数据对比，是否冲突
		for (j = 0; j < attr_info.size(); j++) {
			if (!attr_info[j].primary && !attr_info[j].unique) {
				continue;
			}
			if (att_data[i].type == 1) {
				if (att_data[j].datai == insert_att_data[j].datai) {
					error.info = "Primarykey or uniquekey conflicts";
					return error;
				}
			}
			if (att_data[i].type == 2) {
				if (att_data[j].dataf == insert_att_data[j].dataf) {
					error.info = "Primarykey or uniquekey conflicts";
					return error;
				}
			}
			if (att_data[i].type == 3) {
				if (att_data[j].datas == insert_att_data[j].datas) {
					error.info = "Primarykey or uniquekey conflicts";
					return error;
				}
			}
		}
		free (rawdata);
	}

	//没有冲突，插入数据
	IndexInfo tmp = bmgr.createBlock(filename, 400);
	BYTE array[400] = "";
	//每个attribute的数据插入
	for (i = 0; i < attr_info.size(); i++) {
		Data d = insert_att_data[i];
		char attr_value[400];
		string data;
		if (d.type == 1) {
			data = std::to_string(d.datai);
		}
		if (d.type == 2) {
			data = Convert_ftos(d.dataf);
		}
		else {
			data = d.datas;
		}
		strcpy_s(attr_value, data.c_str());
		strcat_s(array, attr_value);
		//如果是primarykey，直接存index
		if (attr_info[i].attr_name == primarykey_name) {
			if (d.type == 1) {
				iMgr.insert(d.datai, tmp);
				primary_valuei = d.datai;
			}
			if (d.type == 2) {
				iMgr.insert(d.dataf, tmp);
				primary_valuef = d.dataf;
			}
			if (d.type == 3) {
				iMgr.insert(d.datas, tmp);
				primary_values = d.datas;
			}
		}
		//对返回的索引，逐个更新,若该data需要插入index，则更新
		int j;
		for (j = 0; j < index_info.size(); j++) {
			//通过catalog获取indexfilename
			string indexfilename = index_info[j].index_file;
			int n = index_info[j].keyID;
			if (n != i) {
				continue;
			}
			IndexManager Finder(indexfilename, bmgr);
			if (d.type == 1) {
				iMgr.insert(d.datai, Finder.find(primary_valuei));
			}
			if (d.type == 2) {
				iMgr.insert(d.dataf, Finder.find(primary_valuef));
			}
			if (d.type == 3) {
				iMgr.insert(d.datas, Finder.find(primary_values));
			}
		}
	}
	cout << array << endl;
	//std::cout << "No!\n";
}

Error select_tuple(string table_name, vector<std::string> target_name, Where where_select) {
	CatalogManager cmgr;
	Error error;					//返回错误信息
	//Table* table;					//表的信息
	vector<Attribute> attr_info;	//表的所有属性
	vector<index> index_info;		//表的所有index信息
	vector<Tuple_s> return_tuple;
	//获得该表的属性，应在interpreter中判断是否存在表
	/*
		获取表的相关信息，包括attribute、index。
		获取插入的tuple的信息，只包含data的type和值
	*/
	cmgr.getAttrInfo(table_name, attr_info);
	cmgr.getIndex(table_name, index_info);
	//检查是否有错误

	BufferManager bmgr;
	string filename = ".\\";
	filename = filename + table_name;
	filename = filename + ".dat";
	cout << filename << endl;
	int i = 0, j = 0, k = 0;

	string temp;
	//获取primarykey的名称，加入到index_name中，方便之后查找indexfilename
	string index_name = "primary_";
	for (i = 0; i < attr_info.size(); i++) {
		if (attr_info[i].primary == true) {
			index_name = index_name + attr_info[i].attr_name;
			break;
		}
	}
	//用获得的主键的indexname，在该表的index列表中查找indexfilename
	index primarykey;
	for (i = 0; i < index_info.size(); i++) {
		if (index_name == index_info[i].index_name) {
			primarykey = index_info[i];
		}
	}
	//初始化存主键的indexmanager
	IndexManager iMgr(primarykey.index_file, bmgr);

	//声明一个数组来记录该表所有的indexinfo
	vector<IndexInfo> all_indexinfo;
	iMgr.getAllIndex(all_indexinfo);
	BYTE array[400] = "";
	int where_top = 0;
	vector<Tuple_s> select_data;
	//所有tuple逐个对比
	for (i = 0; i < all_indexinfo.size(); i++) {
		//逐个读取rawdata
		char* rawdata = new BYTE[PAGE_SIZE]{ 0 };
		bmgr.readRawData(filename, all_indexinfo[i], rawdata);
		//读取一个tuple的数据
		vector<Data> att_data;
		int offset = 0;
		for (j = 0; j < attr_info.size(); j++) {
			Data temp;
			char *attr_rawdata = getword(offset, attr_info[j].offset, rawdata);
			temp.type = attr_info[j].attr_type;
			//int
			if (temp.type == 1) {
				temp.datai = atoi(attr_rawdata);
			}
			//float
			else if (temp.type == 2) {
				temp.dataf = atof(attr_rawdata);
			}
			//char
			else {
				temp.datas = attr_rawdata;
			}
			att_data.push_back(temp);
			free (attr_rawdata);
		}
		
		for (j = 0; j < attr_info.size();j++) {
			//attribute名相同、且值相同
			if (where_select.attr_name.compare(attr_info[j].attr_name) == 0) {
				if (where_select.data.type == 1) {
					if (where_select.data.datai == att_data[i].datai&&where_select.relation_character == EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datai >= att_data[i].datai&&where_select.relation_character == GREATER_OR_EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datai <= att_data[i].datai&&where_select.relation_character == LESS_OR_EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datai > att_data[i].datai&&where_select.relation_character == GREATER) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datai < att_data[i].datai&&where_select.relation_character == LESS) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
				}
				if (where_select.data.type == 2) {
					if (where_select.data.dataf == att_data[i].datai&&where_select.relation_character == EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.dataf >= att_data[i].dataf&&where_select.relation_character == GREATER_OR_EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.dataf <= att_data[i].dataf&&where_select.relation_character == LESS_OR_EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.dataf > att_data[i].dataf&&where_select.relation_character == GREATER) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.dataf < att_data[i].dataf&&where_select.relation_character == LESS) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
				}
				if (where_select.data.type == 2) {
					if (where_select.data.datas == att_data[i].datas&&where_select.relation_character == EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datas >= att_data[i].datas&&where_select.relation_character == GREATER_OR_EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datas <= att_data[i].datas&&where_select.relation_character == LESS_OR_EQUAL) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datas > att_data[i].datas&&where_select.relation_character == GREATER) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
					else if (where_select.data.datas < att_data[i].datas&&where_select.relation_character == LESS) {
						Tuple_s tuple;
						int k, attr_top = 0;
						//增加select的属性的值到输出的数组中
						for (k = 0; k < att_data.size(); k++) {
							if (attr_info[k].attr_name != target_name[attr_top]) {
								continue;
							}
							tuple.addData(att_data[k]);
							attr_top++;
						}
						select_data.push_back(tuple);
						where_top++;
					}
				}
			}
			//}
		}
		for (i = 0; i < select_data.size(); i++) {
			select_data[i].showTuple();
		}
		//std::cout << "No!\n";
		free (rawdata);
	}
	//这个error甚至没有初始化 By Kevin
	return error;
}

Error delete_tuple(string table_name, Where where_select) {
	CatalogManager cmgr;
	Error error;					//返回错误信息
	//Table* table;					//表的信息
	vector<Attribute> attr_info;	//表的所有属性
	vector<index> index_info;		//表的所有index信息
	vector<Tuple_s> return_tuple;
	//获得该表的属性，应在interpreter中判断是否存在表
	/*
		获取表的相关信息，包括attribute、index。
		获取插入的tuple的信息，只包含data的type和值
	*/
	cmgr.getAttrInfo(table_name, attr_info);
	cmgr.getIndex(table_name, index_info);
	//检查是否有错误

	BufferManager bmgr;
	string filename = ".\\";
	filename = filename + table_name;
	filename = filename + ".dat";
	cout << filename << endl;
	int i = 0, j = 0, k = 0;

	string temp;
	//获取primarykey的名称，加入到index_name中，方便之后查找indexfilename
	string index_name = "primary_";
	for (i = 0; i < attr_info.size(); i++) {
		if (attr_info[i].primary == true) {
			index_name = index_name + attr_info[i].attr_name;
			break;
		}
	}
	//用获得的主键的indexname，在该表的index列表中查找indexfilename
	index primarykey;
	for (i = 0; i < index_info.size(); i++) {
		if (index_name == index_info[i].index_name) {
			primarykey = index_info[i];
		}
	}
	//初始化存主键的indexmanager
	IndexManager iMgr(primarykey.index_file, bmgr);

	//声明一个数组来记录该表所有的indexinfo
	vector<IndexInfo> all_indexinfo;
	iMgr.getAllIndex(all_indexinfo);
	BYTE array[400] = "";
	int where_top = 0;
	vector<Tuple_s> select_data;
	//所有tuple逐个对比
	for (i = 0; i < all_indexinfo.size(); i++) {
		//逐个读取rawdata
		char* rawdata = new BYTE[PAGE_SIZE];
		bmgr.readRawData(filename, all_indexinfo[i], rawdata);
		//读取一个tuple的数据
		vector<Data> att_data;
		int offset = 0;
		for (j = 0; j < attr_info.size(); j++) {
			Data temp;
			char *attr_rawdata = getword(offset, attr_info[j].offset, rawdata);
			temp.type = attr_info[j].attr_type;
			//int
			if (temp.type == 1) {
				temp.datai = atoi(attr_rawdata);
			}
			//float
			else if (temp.type == 2) {
				temp.dataf = atof(attr_rawdata);
			}
			//char
			else {
				temp.datas = attr_rawdata;
			}
			att_data.push_back(temp);
			free (attr_rawdata);
		}
		for (j = 0; j < attr_info.size(); j++) {
			//attribute名相同、且值相同
			if (where_select.attr_name.compare(attr_info[j].attr_name) == 0) {
				if (where_select.data.type == 1) {
					if (where_select.data.datai == att_data[i].datai&&where_select.relation_character == EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datai >= att_data[i].datai&&where_select.relation_character == GREATER_OR_EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datai <= att_data[i].datai&&where_select.relation_character == LESS_OR_EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datai > att_data[i].datai&&where_select.relation_character == GREATER) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datai < att_data[i].datai&&where_select.relation_character == LESS) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
				}
				if (where_select.data.type == 2) {
					if (where_select.data.dataf == att_data[i].datai&&where_select.relation_character == EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.dataf >= att_data[i].dataf&&where_select.relation_character == GREATER_OR_EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.dataf <= att_data[i].dataf&&where_select.relation_character == LESS_OR_EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.dataf > att_data[i].dataf&&where_select.relation_character == GREATER) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.dataf < att_data[i].dataf&&where_select.relation_character == LESS) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
				}
				if (where_select.data.type == 2) {
					if (where_select.data.datas == att_data[i].datas&&where_select.relation_character == EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datas >= att_data[i].datas&&where_select.relation_character == GREATER_OR_EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datas <= att_data[i].datas&&where_select.relation_character == LESS_OR_EQUAL) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datas > att_data[i].datas&&where_select.relation_character == GREATER) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
					else if (where_select.data.datas < att_data[i].datas&&where_select.relation_character == LESS) {
						bmgr.erase(filename, all_indexinfo[i]);
					}
				}
			}
			//}
		}
		free (rawdata);
	}
	error.isError = false;
	error.info = "DELETE TUPLE SUCCESS!";
	return error;
}
	
	
char* getword(int offset, int length, char *rawdata) {
	char *res = new BYTE[length + 2]{ 0 };
		int i;

		for (i = offset; i < offset + length; i++) {
			res[i - offset] = rawdata[i];
		}
		res[i] = '\0';
		return res;
	}
