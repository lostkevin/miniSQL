#pragma once
#include "../interpreter/basic.h"
#include "../RecordManager/RecordManager.h"
#include "../BufferManager/BufferManager.h"
#include "../CatalogManager/catalog.h"


class API {
public:
	API();
	~API();

	Table selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Where> where, char operation);

	int deleteRecord(std::string table_name, vector<Where> where);

	void insertRecord(std::string table_name, Tuple_s& tuple);

	bool createTable(Table );

	bool dropTable(std::string table_name);

	bool createIndex(std::string table_name, std::string index_name, std::string attr_name);

	bool dropIndex(std::string table_name, std::string index_name);

private:
	
};