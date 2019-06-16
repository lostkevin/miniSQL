#include <iostream>
#include "IndexManager.h"

int main () {
	BufferManager bMgr;
	string fileName = "DB.bin";
	IndexManager iMgr (fileName, bMgr);
	iMgr.setIndexInfo (INT);
	for (int i = 1; i <= 10000; i++) {
		if(i%2)iMgr.erase (i);
		//std::cout << iMgr.examine() << std::endl;
	}
	//iMgr.insert (100001, IndexInfo (4096, 0x23456789));
	IndexInfo tmp = iMgr.find (100001);
	std::cout << tmp._size << endl;
	system ("pause");
	return 0;
}