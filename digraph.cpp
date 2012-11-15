/*
 * digraph.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: sinan
 */

#include "digraph.h"

digraph<int> *dgPointer;

int main() {
	dgPointer = new digraph<int>();
	dgPointer->addNode(0);
	dgPointer->addNode(1);
	dgPointer->addNode(2);
	dgPointer->addNode(3);
	//dgPointer->printGraph();
	dgPointer->addLink(1,0);
	dgPointer->addLink(2,1);
	dgPointer->addLink(2,0);
	dgPointer->addLink(0,2);
	dgPointer->addLink(2,3);
	dgPointer->addLink(3,2);
	dgPointer->addLink(1,3);
	dgPointer->addLink(0,3);
	dgPointer->printGraph();

	cout << endl << "Removing node 2!" << endl;

	dgPointer->removeNode(2);
	dgPointer->printGraph();
	cout << endl << "Removing node 0!" << endl;
	dgPointer->removeNode(0);
	dgPointer->removeNode(1);
	dgPointer->removeNode(0);

	dgPointer->printGraph();


	return 0;


















}
