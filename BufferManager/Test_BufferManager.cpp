#include "BufferManager.h"
#include <iostream>


int main () {
	BufferManager bmgr;
	string fileName = ".\\Test.bin";
	string fileName2 = fileName;
	cout << "File is " << (bmgr.IsFileExist (fileName) ? "" : "not ") << "exist" << endl;
	BYTE array[400];	
	for (int i = 1; i <= 10000; i++) {
		IndexInfo tmp = bmgr.createBlock (fileName, 400);
		for (int j = 0; j < 399; j++) {
			array[j] = j % 10 + '0';
		}
		array[399] = '\0';
		char t[100];
		bmgr.readRawData(fileName, tmp, t);
		cout << "1 " << t <<endl;
		bmgr.WriteRawData (fileName, tmp, array);
		cout << "2 " << array <<endl;
		bmgr.readRawData(fileName, tmp, t);
		cout << "3 " << t << endl;
		if (rand () % 2)bmgr.erase (fileName, tmp);
	}

	system ("pause");
	return 0;
}