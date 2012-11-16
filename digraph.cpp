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
	dgPointer->addArc(1,1,4);
	dgPointer->addArc(2,1,6);
	dgPointer->addArc(2,0,4);
	dgPointer->addArc(0,2,5);
	dgPointer->addArc(2,3,8);
	dgPointer->addArc(3,2,2);
	dgPointer->addArc(1,3,4);
	dgPointer->addArc(0,3,9);
	dgPointer->printGraph();

	cout << endl << "Removing node 2!" << endl;

	dgPointer->removeNode(2);
	dgPointer->printGraph();
	cout << endl << "Removing node 3!" << endl;
	dgPointer->removeNode(3);
	dgPointer->printGraph();
	dgPointer->removeNode(0);
	dgPointer->removeNode(1);

	dgPointer->printGraph();


	return 0;


















}
