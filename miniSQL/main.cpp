#include <iostream>
#include <vector>
#include <ctime>
#include "B+Tree_V2.h"
using namespace std;

int main ( ) {
	BPlusTree<int, int> bp;
	int size = 2000000;
	int *a = new int[size];
	for (int i = 0; i < size; i++) {
		a[i] = i;
	}	
	srand (rand () + (uint)time (nullptr));
	for (int i = 0; i < 10 * size; i++) {
		swap (a[rand () % size], a[rand () % size]);
	}
	for (int i = 0; i < size; i++) {
		bp.insert (a[i], new int (a[i] * 2));
	}
	for (int i = 0; i < size; i++) {
		//std::cout << "delete " << a[i] << std::endl;
		bp.erase (a[i]);
		//bp.printData ();
		//std::cout << std::endl << std::endl;
	}
	if (!bp.empty())throw new std::exception ();
	//system ("cls");
	
	
	//bp.printData ();
	//for (int i = 0; i < size; i++) {
	//	if (*bp.find (a[i]) != 2 * a[i])throw new exception("error!");
	//}
	system ("pause");
}
