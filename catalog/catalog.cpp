#ifndef CATALOG_CPP
#define CATALOG_CPP
#endif

#include <algorithm>
#include "catalog.h"

using namespace std;

BufferManager mybuffer;

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

Error Createindex(Index_Info indexInfo) {
	ofstream file;
	Error error;
	index newindex(indexInfo);

	if (!CheckTableExist(newindex.table_name)) {
		error.isError = true;
		error.info = "Table '" + newindex.table_name + "' doesn't exist";
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

Error Dropindex(Index_Info indexInfo) {
	Error error;
	index oldindex;
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

Index_Info readindexinfo(string index_name) {
	Index_Info indexInfo;
	index oldindex;
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

//Record Manager

Error Insert(InsertInfo insertInfo) {
	//std::cout << "My Fault\n";
	Error error;
	int i = 0, j = 0, k = 0;
	Table t;
	t = readtableinfo(insertInfo.tableName);

	Tuple tuple(t);
	string temp;
	char buffer[110000];
	for (i = 0; i < tuple.attr_num; i++) {
		if (insertInfo.element[i].type == 0) {
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
	}
	string result;
	result = tostring(tuple);
	// cout<<result<<endl;
	if (!CheckTableExist(tuple.tablename)) {
		error.isError = true;
		error.info = "Error: Table  '" + tuple.tablename + "' doesn't exist ";
		//printf("Error: Table  \"%s\" doesn't exist \n",tuple.tablename.c_str());
		return error;
	}

	if (t.attr_num != insertInfo.element.size()) {
		error.isError = true;
		error.info = "Error: Column count doesn't match value count";
		//	cout<<"Error: Column count doesn't match value count"<<endl;
		return error;
	}

	string filename = tuple.tablename + "_record";

	int blocknum = Block_num(filename);
	string *avalue = new string[tuple.attr_num*(blocknum + 1)];
	char *rec = (char *)malloc(sizeof(char)*BlockSize);
	string rec1;
	FILE *fp;
	fp = fopen(filename.c_str(), "rb+");
	int start = 0;
	int len = 0;
	int end = 0;

	for (i = 0; i < blocknum + 1; i++) {
		start = 0;
		len = 0;
		end = 0;
		fseek(fp, i*BlockSize, 0);
		fread(rec, sizeof(char), BlockSize, fp);
		rec1 = rec;
		for (k = 0; k < tuple.attr_num - 1; k++) {

			for (j = start; j < rec1.size(); j++) {
				if (rec1.at(j) == ',')
					break;
			}
			end = j;
			len = end - start;
			avalue[i*tuple.attr_num + k] = rec1.substr(start, len);
			start = end + 1;

		}
		avalue[i*tuple.attr_num + k] = rec1.substr(start, rec1.size() - start);
	}
	fclose(fp);
	int flag = 1;
	for (i = 0; i < tuple.attr_num; i++) {
		if (tuple.attr[i].primary == true) {
			for (j = 0; j < blocknum + 1; j++) {
				if (avalue[j*tuple.attr_num + i] == tuple.attr_values[i]) {
					flag = 0;
					break;
				}
			}
		}
		if (tuple.attr[i].unique == true) {
			for (j = 0; j < blocknum + 1; j++) {
				if (avalue[j*tuple.attr_num + i] == tuple.attr_values[i]) {
					flag = 0;
					break;
				}
			}
		}
		if (flag == 0) {
			break;
		}
	}

	if (flag == 1) {

		fp = fopen(filename.c_str(), "rb+");
		if (fp == NULL) {
			cout << "open file failed" << endl;
			return error;
		}

		int offset = blocknum + 1;

		// printf("%d\n",offset);

		fseek(fp, BlockSize*offset, 0);
		fwrite(result.c_str(), sizeof(char), result.length(), fp);
		fclose(fp);

		mybuffer.WriteRawData(filename, fileoffset, 0);

		error.isError = false;
		error.info = "Success: 1row(s) affected";
		//cout<<"Success: 1row(s) affected"<<endl;
		return error;
	}
	else {
		error.isError = true;
		error.info = "Error: Duplicate value in attribute '" + tuple.attr[i].attr_name + "'";
		//printf("Error: Duplicate value in attribute '%s'\n",tuple.attr[i].attr_name.c_str());
		return error;
	}
	delete[] avalue;

	//std::cout << "No!\n";
}

selectError  Select_Delete(SDInfo sdInfo) {
	selectError error;
	error.isError = false;
	int i = 0, j = 0, k = 0;
	vector<selectvalue> element;
	if (!CheckTableExist(sdInfo.tableName)) {
		error.isError = true;
		error.info = "Table  '" + sdInfo.tableName + "' doesn't exist";
		//printf("Table  \"%s\" doesn't exist ",sdInfo.tableName.c_str());
		return error;
	}
	for (i = 0; i < sdInfo.attribute.size(); i++)
	{
		if (!CheckAttrExist(sdInfo.tableName, sdInfo.attribute[i])) {
			error.isError = true;
			error.info = "Unknown column '" + sdInfo.attribute[i] + "' in select clause";
			//	printf("Unknown column '%s' in where clause\n",sdInfo.element[i].rowName.c_str());
			return error;
		}
	}
	for (i = 0; i < sdInfo.element.size(); i++)
	{
		if (!CheckAttrExist(sdInfo.tableName, sdInfo.element[i].rowName)) {
			error.isError = true;
			error.info = "Unknown column '" + sdInfo.element[i].rowName + "' in where clause";
			//	printf("Unknown column '%s' in where clause\n",sdInfo.element[i].rowName.c_str());
			return error;
		}
	}

	string svalue;
	int ivalue;
	float fvalue;



	string filename = sdInfo.tableName + "_record";

	int blocknum = Block_num(filename);


	Table T;
	T = readtableinfo(sdInfo.tableName);


	string *avalue = new string[T.attr_num*(blocknum + 1)];

	char *rec = (char *)malloc(sizeof(char)*BlockSize);

	string rec1;

	FILE *fp;
	fp = fopen(filename.c_str(), "rb+");
	int start = 0;
	int len = 0;
	int end = 0;
	string *record = new string[blocknum + 1];

	for (i = 0; i < blocknum + 1; i++) {
		start = 0;
		len = 0;
		end = 0;
		fseek(fp, i*BlockSize, 0);
		fread(rec, sizeof(char), BlockSize, fp);
		record[i] = rec;
		rec1 = rec;
		//	cout<<rec1<<endl;
		memset(rec, 0, BlockSize);
		for (k = 0; k < T.attr_num - 1; k++) {

			for (j = start; j < rec1.size(); j++) {
				if (rec1.at(j) == ',')
					break;
			}
			end = j;

			len = end - start;

			avalue[i*T.attr_num + k] = rec1.substr(start, len);
			start = end + 1;

		}

		avalue[i*T.attr_num + k] = rec1.substr(start, rec1.size() - start);


	}
	fclose(fp);
	//printf("blocknum= %d\n",blocknum+1);
	int flag = 1;
	int cnt = 0;
	int emptyflag = 0;
	int deletecount = 0;

	for (i = 0; i < blocknum + 1; i++) {
		flag = 1;
		for (j = 0; j < sdInfo.element.size(); j++) {
			for (k = 0; k < T.attr_num; k++) {

				if (T.attr[k].attr_name == sdInfo.element[j].rowName) {
					//printf("%d\n",k);

				   //INT 
					if (T.attr[k].attr_type == INT) {
						ivalue = atoi(avalue[i*T.attr_num + k].c_str());
						if (sdInfo.element[j].type == 0) {//=
							if (sdInfo.element[j].cons.intValue != ivalue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 1) {//!=
							if (sdInfo.element[j].cons.intValue == ivalue) {
								//	printf("%d %d\n",sdInfo.element[j].cons.intValue,ivalue);
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 2) {//<
							if (ivalue >= sdInfo.element[j].cons.intValue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 3) {//>
							if (ivalue <= sdInfo.element[j].cons.intValue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 4) {//<=
							if (ivalue > sdInfo.element[j].cons.intValue) {
								flag = 0;
								break;
							}
						}

						if (sdInfo.element[j].type == 5) {//>=
							if (ivalue < sdInfo.element[j].cons.intValue) {
								flag = 0;
								break;
							}
						}
					}

					// char 
					if (T.attr[k].attr_type == CHAR) {
						svalue = avalue[i*T.attr_num + k];
						if (sdInfo.element[j].type == 0) {//=
							if (sdInfo.element[j].cons.charValue != svalue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 1) {//!=
							if (sdInfo.element[j].cons.charValue == svalue) {
								flag = 0;
								break;
							}
						}
					}
					// float  
					if (T.attr[k].attr_type == FLOAT) {
						fvalue = atof(avalue[i*T.attr_num + k].c_str());
						if (sdInfo.element[j].type == 0) {//=
							if (sdInfo.element[j].cons.floatValue != fvalue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 1) {//!=
							if (sdInfo.element[j].cons.floatValue == fvalue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 2) {//<
							if (fvalue >= sdInfo.element[j].cons.floatValue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 3) {//>
							if (fvalue <= sdInfo.element[j].cons.floatValue) {
								flag = 0;
								break;
							}
						}
						if (sdInfo.element[j].type == 4) {//<=
							if (fvalue > sdInfo.element[j].cons.floatValue) {
								flag = 0;
								break;
							}
						}

						if (sdInfo.element[j].type == 5) {//>=
							if (fvalue < sdInfo.element[j].cons.floatValue) {
								flag = 0;
								break;
							}
						}
					}

				}

			}
			if (flag == 0)
				break;
		}
		if (flag == 1) {
			if (sdInfo.type == 0)//it's select
			{

				emptyflag = 1;
				selectvalue r;
				if (sdInfo.a == 0)
					for (cnt = 0; cnt < T.attr_num; cnt++) {
						//cout<<avalue[i*T.attr_num+cnt]<<" ";
						r.attr_name[cnt] = T.attr[cnt].attr_name;
						r.attr_values[cnt] = avalue[i*T.attr_num + cnt];
					}
				else
				{
					r.count = 0;
					for (cnt = 0; cnt < sdInfo.attribute.size(); cnt++) {
						//cout<<avalue[i*T.attr_num+cnt]<<" ";
						r.attr_name[cnt] = sdInfo.attribute[cnt];
						int k;
						for (k = 0; k < T.attr_num; k++)
							if (T.attr[k].attr_name == sdInfo.attribute[cnt])
								break;
						r.attr_values[cnt] = avalue[i*T.attr_num + k];
						r.count++;
					}
				}
				error.element.push_back(r);
				//cout<<endl;

			}
			else //it's delete
			{
				deletecount++;
				emptyflag = 1;
				record[i] = "";
			}
		}
	}

	if (sdInfo.type == 0) { //select 
	//if(emptyflag==0){
		error.count = error.element.size();
		error.isError = false;
		//error.info="Empty set";
		return error;
		//}  
		//else{
			//error.isError=false;
			//return error;
		//} 
	//	return true;
	}
	if (sdInfo.type == 1) {
		fp = fopen(filename.c_str(), "w");
		if (fp == NULL) {
			cout << "empty file failed" << endl;
		}
		fclose(fp);

		fp = fopen(filename.c_str(), "rb+");

		int writecount = 0;
		for (i = 0; i < blocknum + 1; i++) {
			if (record[i] != "") {
				//cout<<record[i]<<endl;
				fseek(fp, BlockSize*writecount, 0);
				fwrite(record[i].c_str(), sizeof(char), record[i].size(), fp);
				writecount++;
			}
		}

		fclose(fp);
		error.isError = false;
		//ostringstream stream;
		//stream<<deletecount;
		error.count = deletecount;
		//error.info="Success "+stream.str()+" rows affected";
	//	printf("Success %d rows affected\n",deletecount);
		return error;
	}
	delete[] record;
}



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

Tuple searchrecord(int offset, string table_name) {
	Table t;
	t = readtableinfo(table_name);
	Tuple tuple(t);
	string filename = table_name + "_record";
	int blocknum = Block_num(filename);
	if (offset > blocknum) {
		return tuple;
	}

	FILE *fp;
	char *rec = (char *)malloc(sizeof(char)*BlockSize);
	fp = fopen(filename.c_str(), "rb+");
	fseek(fp, offset*BlockSize, 0);
	fread(rec, sizeof(char), BlockSize, fp);
	string result = rec;
	int start = 0;
	int len = 0;
	int  end = 0;
	int i = 0, j = 0;
	for (i = 0; i < t.attr_num; i++) {

		for (j = start; j < result.size(); j++) {
			if (result.at(j) == ',')
				break;
		}
		end = j;
		len = end - start;
		tuple.attr_values[i] = result.substr(start, len);
		start = end + 1;

	}
	return tuple;
}