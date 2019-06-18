#include<iostream>
#include "..\interpreter\interpreter.h"
#include "..\BufferManager\BufferManager.h"
using namespace std;
BufferManager buffer_manager;
int main() {
	cout << "Welcome to our MiniSQL" << endl;
	while (true) {
		Interpreter Query;
		Query.getQuery();
		Query.EXEC();
	}
	return 0;
}