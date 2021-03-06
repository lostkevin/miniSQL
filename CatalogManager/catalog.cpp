#include "catalog.h"

bool CatalogManager::CheckAttrExist (string tablename, string attrname)
{
	auto iter = DatabaseInfo.find (tablename);
	if (iter != DatabaseInfo.end ()) {
		return iter->second.IsAttrExist (attrname);
	}else return false;
}

void CatalogManager::CreateTable (Table newTable)
{
	if (getTableinfo (newTable.tablename)) {
		throw table_exist ();
	}
	DatabaseInfo[newTable.tablename] = newTable;
}

void CatalogManager::DropTable (string table_name)
{
	if (DatabaseInfo.find(table_name) == DatabaseInfo.end()) {
		throw table_not_exist ();
	}
	DatabaseInfo.find (table_name)->second.release();
	DatabaseInfo.erase (table_name);
}

CatalogManager::CatalogManager ()
{
	ifstream fs (".\\Catalog.dat");
	if (fs) {
		uint tableSize = 0;
		fs >> tableSize;
		for (uint i = 0; i < tableSize; i++) {
			uint size = 0;
			fs >> size;
			char * tmpData = new char[size + 10] {0};
			fs.getline (tmpData, size);
			Table tmp = Table (string (tmpData));
			DatabaseInfo[tmp.tablename] = tmp;
			delete tmpData;
		}

	}
}

CatalogManager::~CatalogManager ()
{
	ofstream fs (".\\Catalog.dat");
	uint tableSize = DatabaseInfo.size ();
	fs << tableSize << endl;
	for (auto iter = DatabaseInfo.begin (); iter != DatabaseInfo.end (); iter++) {
		string tmp = iter->second.ToString ();
		uint size = tmp.length () + 3;
		fs << size << " " << tmp << endl;
	}
	fs.close ();
}

bool CatalogManager::CheckTableExist (string table_name)
{
	return DatabaseInfo.find (table_name) != DatabaseInfo.end ();
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

Table::Table (string TableData)
{
	istringstream is(TableData);
	is >> tablename >> table_fileName >> attr_num;
	for (uint i = 0; i < attr_num; i++) {
		is >> attr[i].attr_name >> attr[i].attr_type
			>> attr[i].offset >> attr[i].primary >> attr[i].unique;
	}
	uint size = 0;
	is >> size;
	for (uint i = 0; i < size; i++) {
		index tmp;
		is >> tmp.index_file >> tmp.index_name >> tmp.index_type
			>> tmp.keyID;
		IndexBasic.push_back (tmp);
	}
}

string Table::ToString ()
{
	//表名，表文件名，attr_num，attr, IndexNum, Index
	ostringstream os;
	os << tablename << " " << table_fileName << " " << attr_num << " ";
	for (uint i = 0; i < attr_num; i++) {
		os << attr[i].attr_name << " " << attr[i].attr_type << " "
			<< attr[i].offset << " " << attr[i].primary << " "
			<< attr[i].unique << " ";
	}
	os << IndexBasic.size () << " ";
	for (uint i = 0; i < IndexBasic.size (); i++) {
		os << IndexBasic[i].index_file << " "
			<< IndexBasic[i].index_name << " "
			<< IndexBasic[i].index_type << " "
			<< IndexBasic[i].keyID << " ";
	}
	return os.str();
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

void CatalogManager::CreateIndex (index index, string table_name)
{
	Table* tmp = getTableinfo (table_name);
	if (!tmp) {
		throw table_not_exist ();
	}
	if (index.keyID >= tmp->attr_num) {
		throw attribute_not_exist ();
	}
	if (CheckIndexExist (index.index_name, table_name)) {
		throw index_exist ();
	}
	tmp->IndexBasic.push_back (index);
}

void CatalogManager::Dropindex (string index_name)
{
	Table* table = nullptr;
	for (auto i = DatabaseInfo.begin (); i != DatabaseInfo.end (); i++) {
		index* tmp = i->second.getIndexinfo (index_name);
		if (tmp) {
			table = &(i->second);
			break;
		}
	}
	if (!table) {
		throw index_not_exist ();
	}
	for (auto i = table->IndexBasic.begin(); i != table->IndexBasic.end(); i++) {
		if (i->index_name == index_name) {
			if (table->attr[i->keyID].primary) {
				throw primary_index_drop_fail ();
			}
			remove (i->index_file.c_str ());
			table->IndexBasic.erase (i);
			break;
		}
	}
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
	if (!tmp)throw table_not_exist ();
	for (uint i = 0; i < tmp->attr_num; i++) {
		attributsInfo.push_back (tmp->attr[i]);
	}
}

void CatalogManager::getIndex (string table_name, vector<index>& index)
{
	Table* tmp = getTableinfo (table_name);
	if (tmp) {
		index = tmp->IndexBasic;
	}
}

bool CatalogManager::getUniqueState (string attr_name, string table_name)
{
	Table* tmp = getTableinfo (table_name);
	if (tmp) {
		for (uint i = 0; i < tmp->attr_num; i++) {
			if (tmp->attr[i].attr_name == attr_name) {
				return tmp->attr[i].unique;
			}
		}
	}
	return false;
}

int CatalogManager::getKeyID (string attr_name, string table_name)
{
	Table* tmp = getTableinfo (table_name);
	if (tmp) {
		for (uint i = 0; i < tmp->attr_num; i++) {
			if (tmp->attr[i].attr_name == attr_name) {
				return (int)i;
			}
		}
	}
	return -1;
}

string CatalogManager::getDataFileName (string table_name)
{
	if (!CheckTableExist (table_name)) {
		throw table_not_exist ();
	}
	Table* tmp = getTableinfo (table_name);
	return tmp->table_fileName;
}
