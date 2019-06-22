#include "catalog.h"

bool CatalogManager::CheckAttrExist (string tablename, string attrname)
{
	auto iter = DatabaseInfo.find (tablename);
	if (iter != DatabaseInfo.end ()) {
		return iter->second.IsAttrExist (attrname);
	}else return false;
}

Error CatalogManager::CreateTable (Table newTable)
{
	Error error;
	if (getTableinfo (newTable.tablename)) {
		//printf("Error : Table \"%s\" already exists\n",newtable.tablename.c_str());
		error.isError = true;
		error.info = "Table '" + newTable.tablename + "' already exists";
		return error;
	}
	DatabaseInfo[newTable.tablename] = newTable;
	error.isError = false;
	error.info = "Success : 0 row(s) affected";
	return error;
}

Error CatalogManager::DropTable (string table_name)
{
	Error error;
	if (DatabaseInfo.find(table_name) == DatabaseInfo.end()) {
		error.isError = true;
		error.info = "Unknown Table '" + table_name + "'";
		//printf("Unknown Table \"%s\"\n",tablename.c_str());
		return error;
	}
	DatabaseInfo.find (table_name)->second.release();
	DatabaseInfo.erase (table_name);
	error.isError = false;
	error.info = "Success : 0row(s) affected";
	//printf("Success : 0row(s) affected\n");
	return error;
}

Attribute & Table::getPrimaryKeyInfo ()
{
	uint i = 0;
	for (; i < attr_num; i++) {
		if (attr[i].primary == true) {
			break;
		}
	}
	return attr[i];
}

bool Table::IsAttrExist (string attr_name)
{
	for (uint i = 0; i < attr_num; i++) {
		if (attr[i].attr_name == attr_name)return true;
	}
	return false;
}

index * Table::getIndexinfo (string index_name)
{
	for (uint i = 0; i < this->IndexBasic.size (); i++) {
		if (IndexBasic[i].index_name == index_name)return &IndexBasic[i];
	}
	return nullptr;
}

void Table::release ()
{
	remove (table_fileName.c_str());
	for (uint i = 0; i < IndexBasic.size (); i++) {
		remove (IndexBasic[i].index_file.c_str ());
	}
}

bool CatalogManager::CheckIndexExist (string index_name, string table_name)
{
	Table* tmp = getTableinfo (table_name);
	if (!tmp) return false;
	return tmp->getIndexinfo (index_name);
}

Error CatalogManager::CreateIndex (index index, string table_name)
{
	Error error;
	Table* tmp = getTableinfo (table_name);
	if (!tmp) {
		error.isError = true;
		error.info = "Table '" + table_name + "' doesn't exist";
		return error;
	}
	if (index.keyID >= tmp->attr_num) {
		error.isError = true;
		error.info = "Key column doesn't exist in table";
		return error;
	}
	if (CheckIndexExist (index.index_name, table_name)) {
		error.isError = true;
		error.info = "Duplicate key name '" + index.index_name + "'";
		return error;
	}
	tmp->IndexBasic.push_back (index);
	error.isError = false;
	error.info = "0row(s) affected, Records: 0 ,Duplicates :0, Warning :0";
	return error;
}

Error CatalogManager::Dropindex (string index_name)
{
	Error error;
	Table* table = nullptr;
	for (auto i = DatabaseInfo.begin (); i != DatabaseInfo.end (); i++) {
		index* tmp = i->second.getIndexinfo (index_name);
		if (tmp) {
			table = &(i->second);
			break;
		}
	}
	if (!table) {
		error.isError = true;
		error.info = "index '" + index_name + "' not exist";
		return error;
	}
	for (auto i = table->IndexBasic.begin(); i != table->IndexBasic.end(); i++) {
		if (i->index_name == index_name) {
			remove (i->index_file.c_str ());
			table->IndexBasic.erase (i);
			break;
		}
	}
	error.isError = false;
	error.info = "0row(s) affected, Records: 0 ,Duplicates :0, Warning :0";
	return error;
}

Table * CatalogManager::getTableinfo (string table_name)
{
	auto iter = DatabaseInfo.find (table_name);
	if (iter != DatabaseInfo.end ())return &(iter->second);
	return nullptr;
}

void CatalogManager::getAttrInfo (string table_name, vector<Attribute>& attributsInfo)
{
	Table* tmp = getTableinfo (table_name);
	if (!tmp)throw new exception ("table not exist!");
	for (uint i = 0; i < tmp->attr_num; i++) {
		attributsInfo.push_back (tmp->attr[i]);
	}
}
