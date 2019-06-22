
#include <iostream>
#include "api.h"



void API::selectRecord (string table_name, vector<string> target_attr, vector<Where> where, vector<bool> operations)
{
	if (!where.size ()) throw new exception ("null condition!");
	select_tuple (table_name, target_attr, where, operations);
}

void API::deleteRecord (string table_name, vector<Where> where)
{
	if (!where.size ()) throw new exception ("null condition!");
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
		string tmp = "Table '" + tableInfo.tablename + "' already exists";
		return;
	}
	cMgr.CreateTable (tableInfo);
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
