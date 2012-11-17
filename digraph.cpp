/*
 * digraph.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: sinan
 */

#include "digraph.h"

digraph<char> *dgPointer;

int main() {
	dgPointer = new digraph<char>();
	dgPointer->addNode('A'); //0
	dgPointer->addNode('B'); //1
	dgPointer->addNode('C'); //2
	dgPointer->addNode('D'); //3
	dgPointer->addNode('E'); //4
	dgPointer->addNode('F'); //5
	dgPointer->addNode('G'); //6
	dgPointer->addNode('H'); //7
	dgPointer->addNode('I'); //8

	dgPointer->addArc(0,1);
	dgPointer->addArc(0,3);
	dgPointer->addArc(0,4);
	dgPointer->addArc(1,5);
	dgPointer->addArc(2,1);
	dgPointer->addArc(2,3);
	dgPointer->addArc(3,2);
	dgPointer->addArc(3,7);
	dgPointer->addArc(4,7);
	dgPointer->addArc(5,6);
	dgPointer->addArc(6,1);
	dgPointer->addArc(6,2);
	dgPointer->addArc(8,7);
	dgPointer->addArc(7,6);
	dgPointer->addArc(7,3);

	dgPointer->printGraph();

	dgPointer->solveShortestPath(7,1);


	return 0;


















}
