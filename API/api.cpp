
#include <iostream>
#include "api.h"



void API::selectRecord (string table_name, vector<string> target_attr, vector<Where> where, vector<bool> operations)
{
	select_tuple (table_name, target_attr, where, operations);
}

void API::deleteRecord (string table_name, vector<Where> where)
{
	delete_tuple (table_name, where);
}

void API::insertRecord (string table_name, Tuple_s & tuple)
{
	Insert_tuple (table_name, tuple);
}

void API::createTable (Table tableInfo)
{
	CatalogManager cMgr;
	if (cMgr.CheckTableExist (tableInfo.tablename)) {
		throw table_exist ();
		return;
	}
	uint offset = 0;
	for (uint i = 0; i < tableInfo.attr_num; i++) {
		tableInfo.attr[i].offset = offset;
		//0, 1->4 | 2 -> 2 |  3 -> 3
 		offset += tableInfo.attr[i].attr_type < 2 ? 4 : tableInfo.attr[i].attr_type;
	}
	cMgr.CreateTable (tableInfo);
	//建立主索引
	for (uint i = 0; i < tableInfo.attr_num; i++) {
		if (tableInfo.attr[i].primary) {
			index primaryIndex;
			primaryIndex.index_file = ".\\primary_" + tableInfo.attr[i].attr_name + ".index";
			primaryIndex.index_name = "primary_" + tableInfo.attr[i].attr_name;
			primaryIndex.index_type = tableInfo.attr[i].attr_type;
			primaryIndex.keyID = i;
			cMgr.CreateIndex (primaryIndex, tableInfo.tablename);
			return;
		}
	}

}

void API::dropTable (string table_name)
{
	CatalogManager cMgr;
	if (!cMgr.CheckTableExist (table_name)) {
		cout << ">>> Error: Table '" + table_name + "' does not exist!" << endl;
		return;
	}
	cMgr.DropTable (table_name);
}

void API::createIndex (string table_name, string index_name, string attr_name)
{
	CatalogManager cMgr;
	if (!cMgr.CheckTableExist (table_name)) {
		cout << ">>> Error: Table '" + table_name + "' does not exist!" << endl;
		return;
	}
	if (!cMgr.CheckAttrExist (table_name, attr_name)) {
		cout << ">>> Error: Attribute '"+ attr_name + "' does not exist!" << endl;
		return;
	}
	if (cMgr.CheckIndexExist (index_name, table_name)) {
		cout << ">>> Error: Index  '" + index_name + "' has been created!" << endl;
		return;
	}
	if (!cMgr.getUniqueState (attr_name, table_name)) {
		cout << ">>> Error: Index  '" + index_name + "' is not a unique key!" << endl;
		return;
	}
	vector<Attribute> tmp;
	cMgr.getAttrInfo (table_name, tmp);
	index newIndex;
	newIndex.index_file = ".\\"+ table_name + "_" + index_name + ".index";
	newIndex.index_name = index_name;
	newIndex.keyID = cMgr.getKeyID (attr_name, table_name);
	if (tmp[newIndex.keyID].attr_name != attr_name)
		throw new exception ("Unknown error!");
	newIndex.index_type = tmp[newIndex.keyID].attr_type;
	cMgr.CreateIndex (newIndex, table_name);
}

void API::dropIndex (string index_name, string table_name = string())
{
	CatalogManager cMgr;
	if (!cMgr.CheckIndexExist (index_name, table_name)) {
		cout << ">>> Error: Index  '" + index_name +  "' does not exist!" << endl;
		return;
	}
	cMgr.Dropindex (index_name);
}

void API::eraseIndex (string table_name, Data primary_key)
{
	BufferManager bMgr;
	CatalogManager cMgr;
	vector<index> indexs;
	vector<Attribute> attr_info;
	cMgr.getIndex (table_name, indexs);
	cMgr.getAttrInfo (table_name, attr_info);
	IndexManager iMgr (bMgr);
	for (uint i = 0; i < indexs.size (); i++) {
		if (attr_info[indexs[i].keyID].primary) {
			iMgr.open (indexs[i].index_file);
			break;
		}
	}
	IndexInfo tuple;
	switch (primary_key.type) {
	case -1:
		throw primary_key_null ();
	case 0:tuple = iMgr.find (primary_key.datai); break;
	case 1:tuple = iMgr.find (primary_key.dataf); break;
	default:
		tuple = iMgr.find (primary_key.datas); break;
	}
	char* rawdata = new BYTE[PAGE_SIZE];
	bMgr.readRawData (cMgr.getDataFileName(table_name), tuple, rawdata);
	//读取一个tuple的数据
	vector<Data> att_data;
	for (uint j = 0; j < attr_info.size (); j++) {
		Data temp;
		BYTE* ptr = rawdata + attr_info[j].offset;
		temp.type = attr_info[j].attr_type;
		if (temp.type == -1)
			throw exception ();
		if (temp.type == 0) {
			temp.datai = *(int*)ptr;
		}
		else if (temp.type == 1) {
			temp.dataf = *(float*)ptr;
		}
		else {
			temp.datas = ptr;
		}
		att_data.push_back (temp);
	}
	delete rawdata;
	for (uint i = 0; i < indexs.size (); i++) {
		iMgr.open (indexs[i].index_file);
		switch (att_data[indexs[i].keyID].type) {
		case 0:iMgr.erase (att_data[indexs[i].keyID].datai); break;
		case 1:iMgr.erase (att_data[indexs[i].keyID].dataf); break;
		default:
			iMgr.erase (att_data[indexs[i].keyID].datas);
		}
		iMgr.close ();
	}
}
