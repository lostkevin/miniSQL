#pragma once
#include <vector>
#include "..\miniSQL\Commonheader.h"
#include "../interpreter/basic.h"
#include "../RecordManager/RecordManager.h"
#include "../BufferManager/BufferManager.h"
#include "../CatalogManager/catalog.h"


class API {
public:
	API () {}
	~API () {}


	void selectRecord(string table_name, vector<string>& target_attr, vector<Where>& where, vector<bool>& operation);

	void deleteRecord(string table_name, Where where);

	void insertRecord(string table_name, Tuple_s& tuple);

	void createTable(Table tableInfo);

	void dropTable(string table_name);

	void createIndex(string table_name, string index_name, string attr_name);

	void dropIndex(string index_name, string table_name);

private:
	
};