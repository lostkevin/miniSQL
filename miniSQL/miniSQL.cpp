#include "Commonheader.h"
#include "..\interpreter\interpreter.h"

int main () {
	Interpreter interpreter;
	while (true) {
		interpreter.getQuery ();
		interpreter.EXEC ();
	}
	return 0;
}