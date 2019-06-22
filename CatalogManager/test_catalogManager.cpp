#include <iostream>
#include "catalog.h"

int main () {
	const char * a[10] = {
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9"
	};
	for (uint i = 0; i < 10; i++) {
		CatalogManager cMgr;
		Table test;
		test.attr_num = 3;
		test.tablename = "test_";
		test.tablename += a[i];
		test.table_fileName = ".\\test_";
		test.table_fileName += a[i];
		test.table_fileName += ".dat";
		for (uint j = 0; j < test.attr_num; j++) {
			test.attr[j].attr_name = "test_";
			test.attr[j].attr_name += a[i];
			test.attr[j].attr_name += "_attr_";
			test.attr[j].attr_name += a[j];
			test.attr[j].attr_type = 0;
			test.attr[j].offset = 0;
			test.attr[j].primary = false;
			test.attr[j].unique = false;
		}
		test.attr[0].primary = true;
		test.attr[0].unique = true;
		index testIndex;
		testIndex.index_file = ".\\autotest_";
		testIndex.index_file += a[i];
		testIndex.index_file += ".index";
		testIndex.index_name = "autoindex_";
		testIndex.index_name += a[i];
		testIndex.index_type = 0;
		testIndex.keyID = 0;
		test.IndexBasic.push_back (testIndex);
		cMgr.CreateTable (test);
	}
	
	system ("pause");
	return 0;
}