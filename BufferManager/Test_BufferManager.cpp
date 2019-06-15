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
		for (int j = 0; j < 400; j++) {
			array[j] = i % 256;
		}
		bmgr.WriteRawData (fileName, tmp, array);
		if (rand () % 2)bmgr.erase (fileName, tmp);
	}

	system ("pause");
	return 0;
}