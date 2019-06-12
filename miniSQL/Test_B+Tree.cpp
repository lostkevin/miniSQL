#include <iostream>
#include <vector>
#include <ctime>
#include "B+Tree_V2.h"
using namespace std;

int main ( ) {
	BPlusTree<int, int> *pbp = new BPlusTree<int,int>(3);
	int size = 100000;
	int *a = new int[size];
	for (int i = 0; i < size; i++) {
		a[i] = i;
	}	
	//srand (rand () + (uint)time (nullptr));
	for (int i = 0; i < 10 * size; i++) {
		swap (a[rand () % size], a[rand () % size]);
	}
	for (int i = 0; i < size; i++) {
		pbp->insert (a[i], int (a[i] * 2));
	}
	pbp->printData ();
	for (int i = 0; i < size; i++) {
		//std::cout << "delete " << a[i] << std::endl;
		pbp->erase (a[i]);
		pbp->printData ();
		//std::cout << std::endl << std::endl;
	}
	//if (!pbp->empty())throw new std::exception ();
	//system ("cls");
	delete(pbp);
	pbp = new BPlusTree<int, int> (4);
	for (int i = 0; i < size; i++) {
		a[i] = i;
	}
	srand (rand () + (uint)time (nullptr));
	for (int i = 0; i < 10 * size; i++) {
		swap (a[rand () % size], a[rand () % size]);
	}
	for (int i = 0; i < size; i++) {
		pbp->insert (a[i], int (a[i] * 2));
	}
	for (int i = 0; i < size; i++) {
		//std::cout << "delete " << a[i] << std::endl;
		//pbp->erase (a[i]);
		//bp.printData ();
		//std::cout << std::endl << std::endl;
	}
	//if (!pbp->empty ())throw new std::exception ();
	//system ("cls");
	delete(pbp);
	
	//bp.printData ();
	//for (int i = 0; i < size; i++) {
	//	if (*bp.find (a[i]) != 2 * a[i])throw new exception("error!");
	//}
	system ("pause");
}
