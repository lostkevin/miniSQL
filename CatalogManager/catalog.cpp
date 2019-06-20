#ifndef CATALOG_CPP
#define CATALOG_CPP
#endif

#include <algorithm>
#include "catalog.h"
using namespace std;

BufferManager mybuffer;
//ok
bool CheckAttrExist (string tablename, string attrname)//check if attribute is in the table
{

	string filename = tablename + "_info";
	ifstream file;
	string attr_name;
	string nextline;
	file.open (filename.c_str (), ios::in);

	while (!file.eof ()) {
		file >> attr_name;

		if (attr_name == attrname)
			return true;

		getline (file, nextline);
	}

	return false;
}

/* operations on table */

//ok
bool CheckTableExist (string newtable_name)
{
	ifstream file;
	string tablename;

	file.open ("TableNameList.txt", ios::in);

	if (!file.is_open ()) {
		return false;
	}

	while (!file.eof ()) {
		getline (file, tablename);
		if (tablename == newtable_name) {
			return true;
		}
	}

	file.close ();

	return false;
}
//ok
Error cCreateTable (Table info) {
	Error error;
	Table newtable (info);


	ofstream file;

	if (CheckTableExist (newtable.tablename)) {
		//printf("Error : Table \"%s\" already exists\n",newtable.tablename.c_str());
		error.isError = true;
		error.info = "Table '" + newtable.tablename + "' already exists";
		return error;
	}

	file.open ("TableNameList.txt", ios_base::app);
	file << newtable.tablename << endl;
	file.close ();

	string filename = newtable.tablename + "_info";
	file.open (filename.c_str (), ios::out);

	int i = 0;
	for (i = 0; i < newtable.attr_num; i++) {
		file << newtable.attr[i].attr_name << " "
			<< newtable.attr[i].attr_type << " "
			<< newtable.attr[i].attr_length << " "
			<< newtable.attr[i].primary << " "
			<< newtable.attr[i].unique << endl;
	}
	file.close ();
	filename = newtable.tablename + "_record";
	file.open (filename.c_str (), ios::out);
	file.close ();
	error.isError = false;
	error.info = "Success : 0 row(s) affected";
	//cout<<"Success : 0 row(s) affected"<<endl;
	return error;
}
//ok
Error DropTable (Table info) {
	Error error;
	string tablename = info.tablename;
	if (!CheckTableExist (tablename)) {
		error.isError = true;
		error.info = "Unknown Table '" + tablename + "'";
		//printf("Unknown Table \"%s\"\n",tablename.c_str());
		return error;
	}

	string filename = tablename + "_info";
	remove (filename.c_str ());

	filename = tablename + "_record";
	remove (filename.c_str ());


	ifstream file;
	file.open ("TableNameList.txt", ios::in);
	int i = 0;

	string tablenamelist[32];
	string name;


	while (!file.eof ()) {

		getline (file, tablenamelist[i++]);


	}

	file.close ();

	int pos;

	for (i = 0; i < 32 && !tablenamelist[i].empty (); i++) {
		if (tablename == tablenamelist[i]) {
			pos = i;
			break;
		}
	}



	for (; pos < 32 && !tablenamelist[i].empty (); pos++) {
		tablenamelist[pos] = tablenamelist[pos + 1];
	}



	ofstream file1;
	file1.open ("TableNameList.txt", ios::out);

	for (i = 0; i < 32 && !tablenamelist[i].empty (); i++) {
		file1 << tablenamelist[i] << endl;
	}
	error.isError = false;
	error.info = "Success : 0row(s) affected";
	//printf("Success : 0row(s) affected\n");
	return error;
}


/* operations on index */

//ok
bool CheckIndexExist (string newindexname) {
	ifstream file;
	string indexname;

	file.open ("IndexNameList.txt", ios::in);

	if (!file.is_open ()) {
		return false;
	}

	while (!file.eof ()) {
		getline (file, indexname);
		if (indexname == newindexname) {
			return true;
		}
	}
	file.close ();
	return false;

}
//createindex ok
Error Createindex (index index) {
	ofstream file;
	Error error;


	if (!CheckTableExist (index.table_name)) {
		error.isError = true;
		error.info = "Table '" + index.table_name + "' doesn't exist";
		return error;

	}

	for (int i = 0; i < 5; i++) {	//5 = number of elements in attritube

		if (!CheckAttrExist (index.table_name, index.element[i].attr_name)) {
			error.isError = true;
			error.info = "Key column '" + index.element[i].attr_name + "' doesn't exist in table";
			return error;

		}
	}
	if (CheckIndexExist (index.index_name)) {
		error.isError = true;
		error.info = "Duplicate key name '" + index.index_name + "'";
		return error;

	}

	file.open ("IndexNameList.txt", ios_base::app);

	file << index.index_name << endl;
	file.close ();

	string filename = index.index_name + "_info";
	file.open (filename.c_str (), ios::out);
	file << index.index_name << " " << index.table_name << " ";
	for (int i = 0; i < index.element.size (); i++) {

		file << index.element[i].attr_name;
		if (i != index.element.size () - 1)
			file << " ";

	}
	file << endl;

	file.close ();

	filename = index.index_name + "_record";
	file.open (filename.c_str (), ios::out);
	file.close ();
	error.isError = false;
	error.info = "0row(s) affected, Records: 0 ,Duplicates :0, Warning :0";
	//	printf("0row(s) affected, Records: 0 ,Duplicates :0, Warning :0\n");
	return error;
}
//dropindex ok
Error Dropindex (index indexInfo) {
	Error error;
	index oldindex;
	oldindex.index_name = indexInfo.index_name;
	if (!CheckIndexExist (oldindex.index_name)) {
		error.isError = true;
		error.info = "Can not drop  '" + oldindex.index_name + "',check that column/key exists ";
		//printf("Can not drop  \"%s\", check that column/key exists\n",oldindex.index_name.c_str());
		return error;

	}

	string filename = oldindex.index_name + "_info";

	remove (filename.c_str ());

	filename = oldindex.index_name + "_record";

	remove (filename.c_str ());

	ifstream file;
	file.open ("IndexNameList.txt", ios::in);

	string indexnamelist[100];
	int i = 0;
	while (!file.eof ()) {

		getline (file, indexnamelist[i++]);

	}

	file.close ();
	int pos;

	for (i = 0; i < 100 && !indexnamelist[i].empty (); i++) {
		if (indexnamelist[i] == oldindex.index_name) {

			pos = i;
			break;
		}
	}

	for (; pos < 100 && !indexnamelist[pos].empty (); pos++) {

		indexnamelist[pos] = indexnamelist[pos + 1];
	}

	ofstream file1;

	file1.open ("IndexNameList.txt", ios::out);

	for (i = 0; i < 100 && !indexnamelist[i].empty (); i++) {

		file1 << indexnamelist[i] << endl;
	}

	file1.close ();
	error.isError = 0;
	error.info = "0row(s) affected, Records: 0 ,Duplicates :0, Warning :0";
	//printf("0row(s) affected, Records: 0 ,Duplicates :0, Warning :0\n");
	return error;


}  //ok?
//ok
Table readtableinfo (string table_name) {
	Table table;
	table.tablename = table_name;
	table.attr_num = 0;
	string filename = table_name + "_info";
	ifstream in;
	in.open (filename.c_str (), ios::in);

	if (!in)
	{
		return table;
	}

	string info;
	int k = 0;

	while (!in.eof ())
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
	in.close ();
	return table;
}
//不知道o不ok
index readindexinfo (string index_name) {
	index indexInfo;
	index oldindex;
	oldindex.index_name = index_name;
	if (!CheckIndexExist (oldindex.index_name)) {
		return indexInfo;
	}

	indexInfo.index_name = index_name;

	ifstream in;
	string filename = index_name + "_info";
	in.open (filename.c_str (), ios::in);
	if (!in) {
		cout << "open file failed" << endl;
		return indexInfo;
	}
	string store;

	in >> store;

	in >> store;
	indexInfo.table_name = store;

	Table T;
	T = readtableinfo (indexInfo.table_name);

	while (!in.eof ()) {		//从文件中读index，存储在store中
		in >> store;
		if (in.fail ()) {
			break;
		}
		index r[Asize];
		for (int i = 0; i < T.attr_num; i++) {
			if (store == T.attr[i].attr_name) {
				r[i].index_name = store;
				r[i].attribute.unique = T.attr[i].unique;
				r[i].attribute.primary = T.attr[i].primary;

				break;
			}

		}
	}
	in.close ();
	return indexInfo;
}

