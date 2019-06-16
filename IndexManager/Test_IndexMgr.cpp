#include "IndexManager.h"

int main () {
	BufferManager bMgr;
	string fileName = "DB.bin";
	IndexManager iMgr (fileName, bMgr);
	iMgr.setIndexInfo (INT);

	for (int i = 1; i <= 1000; i++) {
		iMgr.insert (i, IndexInfo (0xffffffff, 8192));
	}
	system ("pause");
	return 0;
}