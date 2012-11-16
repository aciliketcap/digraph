/*
 * digraph.h
 *
 * Light-weight directed graph container
 * Read more about design in aciliketcap.blogger.com
 *
 * Notes:
 * - It uses linked lists to store arcs between nodes. However the linked list
 *   structure is divided between list members (arcLink class) and dgNode class holding
 *   the top/beginning elements of the link. Basically a list member can only be
 *   deleted by a predecessor (since each member has only forward links). So if you
 *   want to delete a top/beginning node of a list you have to do it in dgNode class.
 *   (It is really bad design, I should have made a list class instead I made a list
 *   element class)
 * - It always removes the first matching element in lists but adds elements in
 *   different places. So if there are more than one identical elements you can't be
 *   sure which one will be removed. If you have more than one arcs from one node to
 *   another (with different weights) you should use removeArcW(src, dest, weight).
 *
 */

#ifndef DIGRAPH_H_
#define DIGRAPH_H_

using namespace std;
#include <iostream>
#include <list>
#include <vector>

template <typename elementType>
class digraph {
public:
	digraph();
	int addNode(elementType data); //return index of the added node
	void removeNode(int nodeIndex);
	void addArc(int srcNodeIndex, int dstNodeIndex, int weight=1); //add a new arc to "srcNode"
	void removeArc(int srcNodeIndex, int dstNodeIndex); //remove an arc from "srcNode"
	void removeArcW(int srcNodeIndex, int dstNodeIndex, int weight=0); //remove an arc from "srcNode" whose weight is equal to weight
	void printGraph(); //TODO: move this to << operator later
	void removeAllArcsTo(int nodeIndex); //for debug purposes, remove all arcs directed at a node
private:
	class dgNode {
	public:
		dgNode(elementType data);
		dgNode(elementType data, dgNode* adjNode);
		//TODO: we can use a destructor which calls remove arcs to and from functions on itself
		void addArc(dgNode* adjNode, int weight=1); //add new arc to node at the end of arcs list
		void removeArc(dgNode* adjNode); //remove arc to node from the arcs list, remove link to the source node too!
		void removeArcW(dgNode* adjNode, int weight=1); //same as above but also check weight
		void removeArcsToThis(); //remove all arcs coming to node from other nodes
		void removeArcsFromThis(); //remove all arcs sent from this node and their references on respective nodes
		int getNumArcs();
		int getNumArcsFrom();
		elementType getData();
		/* This is so weird, I have to declare these functions inside the class or else it doesn't compile
		 * They say "outside the class definition is a non-deducible context" on the net.*/
		friend ostream& operator<<(ostream& os, const digraph<elementType>::dgNode& node){
			digraph<elementType>::dgNode oNode = node; //take a copy of const node so we can use functions
			os << "Data: " << oNode.getData() << " @[" << &node << "]" << endl; //use &node to print original node addr
			os << "Has " << oNode.getNumArcs() <<" arcs to: " << *oNode.firstArcToOther << " " << endl;
			os << "Has " << oNode.getNumArcsFrom() << " arcs coming from: " << *oNode.topArcFromOther << endl;
		};
	private:
		class arcLink { //linked list of arcs (pointers to end/destination nodes)
		public:
			arcLink(dgNode *adjNode, int weight=1); //create an unbound arc (the first element of a arc list)
			arcLink(dgNode *adjNode, arcLink *previous, int weight=1); //create and place after *previous
			~arcLink(); //subtract 1 from all forward indices
			arcLink* removeArcToNode(dgNode *adjNode); //find the first arc (if there is more than one) pointing to adjNode and remove it
			arcLink* removeArcToNodeW(dgNode *adjNode, int weight=1); //same as above but also check weight
			inline dgNode *getNode() {return this->adjNode;} //return the node the arc points to
			//TODO: getArc function is useless, get rid of it.
			//dgNode *getArc(int index); //give the node pointed by arc at index, just like getNode
			int getLastIndex(); //iterate elements one by one and return the last one's index
			inline int getIndex() {return this->index;}
			inline int getWeight() {return this->weight;}
			inline arcLink *getNextLink() {return this->nextLink;}
			//must define << here, other places are "non-deducible context" according to compiler
			friend ostream& operator<<(ostream& os, digraph<elementType>::dgNode::arcLink& firstLink) {
				arcLink *temp = &firstLink;
				while(temp) {
					os << "(" << temp->weight <<  ")[" << temp->adjNode << "] ";
					temp = temp->nextLink;
				}
				os << temp; //print 0 at the end, so that we know that we reached the end of the list.
			}; //print from first to last

		private:
			dgNode *adjNode;
			arcLink *nextLink;
			int index; //holds the position of the arc in the list (therefore last items index is the size of the list)
			int weight; //arc weight
		};
		arcLink *firstArcToOther; //pointer to first arc in linked list of graph arcs from this node TO other nodes
		arcLink *lastArcToOther; //just not to traverse whole list of arcs when we just need to check last one
		arcLink *topArcFromOther; //the list of nodes who sends arcs to this. Makes node removal easier and faster

		elementType data; //the actual data stored in the node, it is a container after all

	};
	list<dgNode> nodeList;
	vector<dgNode*> indexList; //holds pointers to nodes so that we can access them randomly via their indices
};

/*
 * use it only for initializing the first element of the linked list
 * creates a list element without a predecessor
 */
template <typename elementType>
digraph<elementType>::dgNode::arcLink::arcLink(dgNode *adjNode, int weight) {
	this->adjNode = adjNode;
	this->nextLink = 0;
	this->index = 1;
	this->weight = weight;
}

/*
 * this function should only be used when previous != 0
 * or else previous remains 0 and we can't add any links behind it
 * these arcLink objects can't be used as a linked list directly!
 */
template <typename elementType>
digraph<elementType>::dgNode::arcLink::arcLink(dgNode *adjNode, arcLink *previous, int weight) {
	arcLink *temp;
	this->adjNode = adjNode;
	this->weight = weight;
	if(previous==0) cerr << "cannot add link, previous is non-defined" << endl;
	else {
		this->nextLink = previous->nextLink;
		previous->nextLink = this;
		index = previous->index + 1;
		temp = nextLink;
		while(temp) {
			(temp->index)++;
			temp=temp->nextLink;
		}
	}
}

/*
 * pointing previous elements link to the element after this one should be done first!
 * It is not the responsibility of destructor, it just adjusts the indices
 */
template <typename elementType>
digraph<elementType>::dgNode::arcLink::~arcLink() {
	arcLink *temp = nextLink;
	while(temp) {
		temp->index--;
		temp=temp->nextLink;
	}
}

/*
 * The problem with this function is "what if we need to remove ourself?"
 * If the node we are trying to remove is "this" node,
 * unable to remove itself without severing the whole list, it returns 0.
 * Then dgNode object calling this function should do the removal.
 *
 * Returns the previous list element (predecessor of the removed link) otherwise
 */
template <typename elementType>
//implementing nested templates comes with interesting grammar
//we have to state explicitly that digraph<elementType>::dgNode::arcLink* below is a typename by writing "typename" before it.
//Or else we get compiler error: error: need ‘typename’ before ‘digraph<elementType>::dgNode::arcLink’ because ‘digraph<elementType>’ is a dependent scope
typename digraph<elementType>::dgNode::arcLink* digraph<elementType>::dgNode::arcLink::removeArcToNodeW(dgNode *adjNode, int weight) {
	arcLink *temp = this;
	arcLink *remove;
	if(this->adjNode == adjNode && this->weight == weight) {
		return 0; //can't remove self
	}
	while(temp->nextLink) {
		if(temp->nextLink->adjNode == adjNode && this->weight == weight) {
			remove = temp->nextLink;
			temp->nextLink = remove->nextLink;
			delete remove; //reduce all forward indices
			return temp; //remove just one link and return its predecessor
		}
		else temp = temp->nextLink;
	}
	cerr << "Arc to node " << adjNode << " cannot be found in list containing" << this << endl;
	//TODO: raise exception here! (when I implement an exception mechanism)
}

//same as above but don't consider weight, just remove the first arc you encounter
template <typename elementType>
typename digraph<elementType>::dgNode::arcLink* digraph<elementType>::dgNode::arcLink::removeArcToNode(dgNode *adjNode) {
	arcLink *temp = this;
	arcLink *remove;
	if(this->adjNode == adjNode) {
		return 0; //can't remove self
	}
	while(temp->nextLink) {
		if(temp->nextLink->adjNode == adjNode) {
			remove = temp->nextLink;
			temp->nextLink = remove->nextLink;
			delete remove; //reduce all forward indices
			return temp; //remove just one link and return its predecessor
		}
		else temp = temp->nextLink;
	}
	cerr << "Arc to node " << adjNode << " cannot be found in list containing" << this << endl;
	//TODO: raise exception here! (when I implement an exception mechanism)
}


/*
 * not sure if this function is useful in general.
 * returns a directed arc from the arcsTo list with index.
 * only useful for graphs in which order of arcs matters.
 * But we don't care about the order of arcs by design
 */
/*template <typename elementType>
typename digraph<elementType>::dgNode* digraph<elementType>::dgNode::arcLink::getArc(int index) {
	arcLink *temp = this;
	if(index<this->index) return 0;
	else {
		while(temp->index != index) {
			temp=temp->nextLink;
			if(temp == 0) break; //end of list
		}
	}
	return temp;
}*/

template <typename elementType>
int digraph<elementType>::dgNode::arcLink::getLastIndex() {
	arcLink *temp;
	temp = this;
	/*
	 * If the list is empty, getLastIndex() is called from an arcLink object
	 * whose address is 0. More correctly it is called via a pointer to an
	 * arcLink object whose value is 0.
	 * Hence we protect ourself by checking if 'this' is non-zero
	 */
	if(temp != 0) {
		while(temp->nextLink != 0) temp = temp->nextLink;
		return temp->index; //return last element's index value
	}
	return 0;
}

template <typename elementType>
digraph<elementType>::dgNode::dgNode(elementType data) {
	this->data = data;
	//init empty linked lists
	this->firstArcToOther = 0;
	this->lastArcToOther = 0;
	this->topArcFromOther = 0;
}

//init arcsToOther linked list with first (and last) element containing *adjnode
template <typename elementType>
digraph<elementType>::dgNode::dgNode(elementType data, dgNode *adjNode) {
	this->data = data;
	firstArcToOther = new arcLink(adjNode);
	lastArcToOther = firstArcToOther;
	this->topArcFromOther = 0;
}

//add an arc to adjNode at the end of linked list of arcs
template <typename elementType>
void digraph<elementType>::dgNode::addArc(dgNode *adjNode, int weight) {
	if(lastArcToOther == 0) {
		lastArcToOther = new arcLink(adjNode, weight);
		firstArcToOther = lastArcToOther;
	}
	else lastArcToOther = new arcLink(adjNode, lastArcToOther, weight);
	//create a link to this node in the destination node too, to make removal of nodes easier
	if(adjNode->topArcFromOther == 0) adjNode->topArcFromOther = new arcLink(this, weight); //initializes the list topArcFromOther in a sense
	else new arcLink(this, adjNode->topArcFromOther, weight); //just add the new link at the end of the list beginning with topArcFromOther
}

template <typename elementType>
void digraph<elementType>::dgNode::removeArc(dgNode *adjNode) {
	arcLink *remove;
	arcLink *previous;
	int lastIndex = lastArcToOther->getIndex();
	if(adjNode->topArcFromOther) { //let's check the list of incoming arcs on other side
		if(!adjNode->topArcFromOther->removeArcToNode(this)) {
			//we need to remove first link in the list (it can't be done by using arcLink's functions)
			remove = adjNode->topArcFromOther;
			adjNode->topArcFromOther = remove->getNextLink(); //make the second link in the list new head of the list
			delete remove; //destructor of remove adjusts the indices for the remaining of the list
		}
	}
	else cerr << "Can't find the reference to arc in " << adjNode << " from " << this << ". Incoming arcs list is empty!" << endl;
	//TODO: we should raise exception here when we have an exception mechanism
	if(this->firstArcToOther) {
		previous = this->firstArcToOther->removeArcToNode(adjNode);
		if(!previous) {
			//again we have to remove the first link by hand
			remove = firstArcToOther;
			firstArcToOther = remove->getNextLink(); //make second link the first link
			delete remove; //get rid of the first link and adjust the indices
			if(firstArcToOther == 0) lastArcToOther = firstArcToOther; //all elements in the list have been removed
		}
		else if(previous->getNextLink()==0) lastArcToOther = previous; //we have deleted the end of the list and have to update lastArcToOther
	}
	else cerr << "There is no arc to *" << adjNode << " to remove!" << endl;
	return; //we can change the return type and return false here.
}

/*
 * same as above only checks weight too
 * If you have more than one arc from one node to another with different weights
 * You should use this to remove a specific arc. Useless in other cases.
 */
template <typename elementType>
void digraph<elementType>::dgNode::removeArcW(dgNode *adjNode, int weight) {
	arcLink *remove;
	arcLink *previous;
	int lastIndex = lastArcToOther->getIndex();
	if(adjNode->topArcFromOther) { //let's check the list of incoming arcs on other side
		if(!adjNode->topArcFromOther->removeArcToNodeW(this, weight)) {
			//we need to remove first link in the list (it can't be done by using arcLink's functions)
			remove = adjNode->topArcFromOther;
			adjNode->topArcFromOther = remove->getNextLink(); //make the second link in the list new head of the list
			delete remove; //destructor of remove adjusts the indices for the remaining of the list
		}
	}
	else cerr << "Can't find the reference to arc in " << adjNode << " from " << this << ". Incoming arcs list is empty!" << endl;
	//TODO: we should raise exception here when we have an exception mechanism
	if(this->firstArcToOther) {
		previous = this->firstArcToOther->removeArcToNode(adjNode);
		if(!previous) {
			//again we have to remove the first link by hand
			remove = firstArcToOther;
			firstArcToOther = remove->getNextLink(); //make second link the first link
			delete remove; //get rid of the first link and adjust the indices
			if(firstArcToOther == 0) lastArcToOther = firstArcToOther; //all elements in the list have been removed
		}
		else if(previous->getNextLink()==0) lastArcToOther = previous; //we have deleted the end of the list and have to update lastArcToOther
	}
	else cerr << "There is no arc to *" << adjNode << " to remove!" << endl;
	return; //we can change the return type and return false here.
}
/*
 * severe all arcs coming to the node
 * this part is the same as the second part of removeArc(adjNode) function
 * only it doesn't work on this node's list, it works on other nodes' lists
 */
template <typename elementType>
void digraph<elementType>::dgNode::removeArcsToThis() {
	typename dgNode::arcLink *temp, *previous;
	while(topArcFromOther) {
		previous = topArcFromOther->getNode()->firstArcToOther->removeArcToNode(this);
		if(!previous) {
			temp = topArcFromOther->getNode()->firstArcToOther;
			topArcFromOther->getNode()->firstArcToOther = topArcFromOther->getNode()->firstArcToOther->getNextLink();
			delete temp; //delete the link on the other side
			if(topArcFromOther->getNode()->firstArcToOther == 0)  topArcFromOther->getNode()->lastArcToOther = topArcFromOther->getNode()->firstArcToOther;
		}
		else if(previous->getNextLink() == 0) topArcFromOther->getNode()->lastArcToOther = previous;
		temp = topArcFromOther;
		topArcFromOther = topArcFromOther->getNextLink();
		delete temp; //delete the incoming arc reference on the other side we just deleted
	}
}

/*
 * remove all arcs originating from this node
 */
template <typename elementType>
void digraph<elementType>::dgNode::removeArcsFromThis() {
	typename dgNode::arcLink *temp = lastArcToOther;
	while(temp) {
		this->removeArc(temp->getNode());
		temp = lastArcToOther;
	}
}

template <typename elementType>
int digraph<elementType>::dgNode::getNumArcs() {
	if(lastArcToOther == 0) return 0;
	return lastArcToOther->getIndex();
}

//get the number of arcs from other nodes to this node
//this function traverses the list of incoming arcs
template <typename elementType>
int digraph<elementType>::dgNode::getNumArcsFrom() {
	return this->topArcFromOther->getLastIndex();
}

template <typename elementType>
elementType digraph<elementType>::dgNode::getData() {
	return this->data;	//may be we should return a reference?
}

template <typename elementType>
digraph<elementType>::digraph() {
	//do we really need a constructor?
}

template <typename elementType>
int digraph<elementType>::addNode(elementType data) {
	nodeList.push_back(dgNode(data));
	indexList.push_back(&nodeList.back());
	return nodeList.size();
}

/*
 * Note that removing a node may be expensive:
 * 1) You meet each node that arcs to the node to be removed,
 *        traverse their linked lists of outgoing arcs and
 *        remove arcs(pointers) to the node.
 * 2) You meet each node the node has an arc to,
 *        traverse their linked lists of incoming arcs and
 *        remove pointers to the node to be removed.
 */
template <typename elementType>
void digraph<elementType>::removeNode(int nodeIndex) {
	dgNode* remove = indexList[nodeIndex];
	remove->removeArcsToThis();
	remove->removeArcsFromThis(); //delete all arcs in the node, more importantly delete references to this node in other nodes
	typename list<dgNode>::iterator it;
	for(it = nodeList.begin(); it!=nodeList.end();it++){
		if(&*it==remove) {
			break;
			cout << "node [" << remove << "] [" << &*it << "] found. Removing..." << endl;
		}
	}
	nodeList.erase(it); //now we can rest easy
	indexList[nodeIndex] = 0; //put a zero so if anyone tries to access the node via its index, we can tell them the node was deleted.

	return;
}

template <typename elementType>
void digraph<elementType>::addArc(int srcNodeIndex, int dstNodeIndex, int weight) {
	indexList[srcNodeIndex]->addArc(indexList[dstNodeIndex], weight);
	return;
}

template <typename elementType>
void digraph<elementType>::removeArc(int srcNodeIndex, int dstNodeIndex) {
	indexList[srcNodeIndex]->removeArc(indexList[dstNodeIndex]);
	return;
}

template <typename elementType>
void digraph<elementType>::removeArcW(int srcNodeIndex, int dstNodeIndex, int weight) {
	indexList[srcNodeIndex]->removeArc(indexList[dstNodeIndex]);
	return;
}

template <typename elementType>
void digraph<elementType>::printGraph() {
	if(nodeList.empty()) {
		cout << "The graph is empty!" << endl;
		return;
	}
	cout << "Here is the index list:" << endl;
	int nodeNumber = 0;
	typename vector<dgNode*>::iterator index;
	for(index = indexList.begin(); index!=indexList.end(); index++) {
		cout << "(" << nodeNumber << ")@[" << *index << "] ";
		nodeNumber++;
	}
	cout << endl;
	nodeNumber=0;
	typename list<dgNode>::iterator it;
	for(it = nodeList.begin(); it!=nodeList.end(); it++) {
		cout << "Node Index: " << nodeNumber << " " << *it;
		nodeNumber++;
	}
	return;
}

template <typename elementType>
void digraph<elementType>::removeAllArcsTo(int nodeIndex) {

	typename list<dgNode>::iterator it = nodeList.begin();
	advance(it,nodeIndex);
	(*it).removeArcsToThis();
}

#endif /* DIGRAPH_H_ */
