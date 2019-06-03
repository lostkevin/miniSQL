#include <iostream>
#include <vector>
#include "B+Tree.h"
using namespace std;

int main ( ) {
	BPlusTree<int,int> bp(6);
	int size = 200;
	int *a = new int[size];
	for (int i = 0; i < size; i++) {
		a[i] = i;
	}	
	for (int i = 0; i < 2*size; i++) {
		swap (a[rand ( ) % size], a[rand ( ) % size]);
	}
	for (int i = 0; i < size; i++) {
		bp.insert ((int)a[i], new int (a[i] * 2));
	}
	for (int i = 0; i < size; i++) {
		cout << *bp.find (i) << endl;
	}
	system ("pause");
}
