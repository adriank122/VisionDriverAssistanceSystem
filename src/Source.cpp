/*
VISION DRIVER ASSISTANCE SYSTEM

BY ADRIAN KALICIECKI

2019
*/

#include "Interface.h"

int main() {

	Interface interface;

	interface.loadInput();
	interface.showMenu();
	interface.startProgram();

	return 0;
}