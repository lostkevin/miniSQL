#ifndef CATALOG_CPP
#define CATALOG_CPP
#endif

#include <algorithm>
#include "catalog.h"

using namespace std;
bool CheckAttrExist(string tablename, string attrname)//check if attribute is in the table
{

	string filename = tablename + "_info";
	ifstream file;
	string attr_name;
	string nextline;
	file.open(filename.c_str(), ios::in);

	while (!file.eof()) {
		file >> attr_name;

		if (attr_name == attrname)
			return true;

		getline(file, nextline);
	}

	return false;
}

/* operations on table */


bool CheckTableExist(string newtable_name)
{
	ifstream file;
	string tablename;

	file.open("TableNameList.txt", ios::in);

	if (!file.is_open()) {
		return false;
	}

	while (!file.eof()) {
		getline(file, tablename);
		if (tablename == newtable_name) {
			return true;
		}
	}

	file.close();

	return false;
}

Error cCreateTable(TableInfo info) {
	Error error;
	Table newtable(info);


	ofstream file;

	if (CheckTableExist(newtable.tablename)) {
		//printf("Error : Table \"%s\" already exists\n",newtable.tablename.c_str());
		error.isError = true;
		error.info = "Table '" + newtable.tablename + "' already exists";
		return error;
	}

	file.open("TableNameList.txt", ios_base::app);
	file << newtable.tablename << endl;
	file.close();

	string filename = newtable.tablename + "_info";
	file.open(filename.c_str(), ios::out);

	int i = 0;
	for (i = 0; i < newtable.attr_num; i++) {
		file << newtable.attr[i].attr_name << " "
			<< newtable.attr[i].attr_type << " "
			<< newtable.attr[i].attr_length << " "
			<< newtable.attr[i].primary << " "
			<< newtable.attr[i].unique << endl;
	}
	file.close();
	filename = newtable.tablename + "_record";
	file.open(filename.c_str(), ios::out);
	file.close();
	error.isError = false;
	error.info = "Success : 0 row(s) affected";
	//cout<<"Success : 0 row(s) affected"<<endl;
	return error;
}

Error DropTable(TableInfo info) {
	Error error;
	string tablename = info.tableName;
	if (!CheckTableExist(tablename)) {
		error.isError = true;
		error.info = "Unknown Table '" + tablename + "'";
		//printf("Unknown Table \"%s\"\n",tablename.c_str());
		return error;
	}

	string filename = tablename + "_info";
	remove(filename.c_str());

	filename = tablename + "_record";
	remove(filename.c_str());


	ifstream file;
	file.open("TableNameList.txt", ios::in);
	int i = 0;

	string tablenamelist[32];
	string name;


	while (!file.eof()) {

		getline(file, tablenamelist[i++]);


	}

	file.close();

	int pos;

	for (i = 0; i < 32 && !tablenamelist[i].empty(); i++) {
		if (tablename == tablenamelist[i]) {
			pos = i;
			break;
		}
	}



	for (; pos < 32 && !tablenamelist[i].empty(); pos++) {
		tablenamelist[pos] = tablenamelist[pos + 1];
	}



	ofstream file1;
	file1.open("TableNameList.txt", ios::out);

	for (i = 0; i < 32 && !tablenamelist[i].empty(); i++) {
		file1 << tablenamelist[i] << endl;
	}
	error.isError = false;
	error.info = "Success : 0row(s) affected";
	//printf("Success : 0row(s) affected\n");
	return error;
}


/* operations on index */


bool CheckIndexExist(string newindexname) {
	ifstream file;
	string indexname;

	file.open("IndexNameList.txt", ios::in);

	if (!file.is_open()) {
		return false;
	}

	while (!file.eof()) {
		getline(file, indexname);
		if (indexname == newindexname) {
			return true;
		}
	}
	file.close();
	return false;

}

Error Createindex(IndexInfo indexInfo) {
	ofstream file;
	Error error;
	Index newindex(indexInfo);
	/*for(int i=0;i<newindex.element.size();i++){
		cout<<newindex.element[i].attr_name<<endl;
	}*/
	if (!CheckTableExist(newindex.table_name)) {
		error.isError = true;
		error.info = "Table '" + newindex.table_name + "' doesn't exist";
		//printf("Table \"%s\" doesn't exist\n",newindex.table_name.c_str());
		return error;

	}

	for (int i = 0; i < newindex.element.size(); i++) {

		if (!CheckAttrExist(newindex.table_name, newindex.element[i].attr_name)) {
			error.isError = true;
			error.info = "Key column '" + newindex.element[i].attr_name + "' doesn't exist in table";
			//	printf("Key column \"%s\" doesn't exist in table\n",newindex.attr_name.c_str());
			return error;

		}
	}
	if (CheckIndexExist(newindex.index_name)) {
		error.isError = true;
		error.info = "Duplicate key name '" + newindex.index_name + "'";
		// printf("Duplicate key name \"%s\"\n",newindex.index_name.c_str());
		return error;

	}

	file.open("IndexNameList.txt", ios_base::app);

	file << newindex.index_name << endl;
	file.close();

	string filename = newindex.index_name + "_info";
	file.open(filename.c_str(), ios::out);
	file << newindex.index_name << " " << newindex.table_name << " ";
	for (int i = 0; i < newindex.element.size(); i++) {

		file << newindex.element[i].attr_name;
		if (i != newindex.element.size() - 1)
			file << " ";

	}
	file << endl;

	file.close();

	filename = newindex.index_name + "_record";
	file.open(filename.c_str(), ios::out);
	file.close();
	error.isError = false;
	error.info = "0row(s) affected, Records: 0 ,Duplicates :0, Warning :0";
	//	printf("0row(s) affected, Records: 0 ,Duplicates :0, Warning :0\n");
	return error;
}

Error Dropindex(IndexInfo indexInfo) {
	Error error;
	Index oldindex;
	oldindex.index_name = indexInfo.indexName;
	if (!CheckIndexExist(oldindex.index_name)) {
		error.isError = true;
		error.info = "Can not drop  '" + oldindex.index_name + "',check that column/key exists ";
		//printf("Can not drop  \"%s\", check that column/key exists\n",oldindex.index_name.c_str());
		return error;

	}

	string filename = oldindex.index_name + "_info";

	remove(filename.c_str());

	filename = oldindex.index_name + "_record";

	remove(filename.c_str());

	ifstream file;
	file.open("IndexNameList.txt", ios::in);

	string indexnamelist[100];
	int i = 0;
	while (!file.eof()) {

		getline(file, indexnamelist[i++]);

	}

	file.close();
	int pos;

	for (i = 0; i < 100 && !indexnamelist[i].empty(); i++) {
		if (indexnamelist[i] == oldindex.index_name) {

			pos = i;
			break;
		}
	}

	for (; pos < 100 && !indexnamelist[pos].empty(); pos++) {

		indexnamelist[pos] = indexnamelist[pos + 1];
	}

	ofstream file1;

	file1.open("IndexNameList.txt", ios::out);

	for (i = 0; i < 100 && !indexnamelist[i].empty(); i++) {

		file1 << indexnamelist[i] << endl;
	}

	file1.close();
	error.isError = 0;
	error.info = "0row(s) affected, Records: 0 ,Duplicates :0, Warning :0";
	//printf("0row(s) affected, Records: 0 ,Duplicates :0, Warning :0\n");
	return error;


}

Table readtableinfo(string table_name) {
	Table table;
	table.tablename = table_name;
	table.attr_num = 0;
	string filename = table_name + "_info";
	ifstream in;
	in.open(filename.c_str(), ios::in);

	if (!in)
	{
		return table;
	}

	string info;
	int k = 0;

	while (!in.eof())
	{
		in >> table.attr[k].attr_name;
		in >> table.attr[k].attr_type;
		in >> table.attr[k].attr_length;
		in >> table.attr[k].primary;
		in >> table.attr[k].unique;
		if (table.attr[k].primary == 1) {
			table.primary_key = table.attr[k].attr_name;
		}
		k++;
	}
	table.attr_num = k - 1;
	in.close();
	return table;
}

IndexInfo readindexinfo(string index_name) {
	IndexInfo indexInfo;
	Index oldindex;
	oldindex.index_name = index_name;
	if (!CheckIndexExist(oldindex.index_name)) {
		return indexInfo;
	}
	indexInfo.indexName = index_name;
	ifstream in;
	string filename = index_name + "_info";
	in.open(filename.c_str(), ios::in);
	if (!in) {
		cout << "open file failed" << endl;
		return indexInfo;
	}
	string store;

	in >> store;

	in >> store;
	indexInfo.tableName = store;

	Table T;
	T = readtableinfo(indexInfo.tableName);

	while (!in.eof()) {
		in >> store;
		if (in.fail()) {
			break;
		}
		RowInfo r;
		for (int i = 0; i < T.attr_num; i++) {
			if (store == T.attr[i].attr_name) {
				r.rowName = store;
				/*	if(T.attr[i].attr_type==INT){
						r.type=Int;
					}
					if(T.attr[i].attr_type==CHAR){
						r.type=Char;
						r.charSize=T.attr[i].attr_length;
					}
					if(T.attr[i].attr_type==FLOAT){
						r.type=Float;
					} */
				r.isUnique = T.attr[i].unique;
				r.isPrimary = T.attr[i].primary;
				indexInfo.element.push_back(r);
				break;
			}

		}
	}
	in.close();
	return indexInfo;
}
