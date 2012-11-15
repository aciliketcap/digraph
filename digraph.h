/*
 * digraph.h
 *
 * Directed graph container (with pointers)
 *
 */

/*
 * I learned three wonderful things today:
 * You can't change the pointer to an object by using this=somepointer; inside a function to that object
 * You can't change where a pointer points inside a function
 * which came from arguments of the function
 * because parameters are passed by reference
 * and the pointer value you are trying to change is just
 * a copy of original pointer outside the function scope
 * You can delete an object inside one of its member functions
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
	/*
	 * use index numbers for nodes, their index in the node vector
	 * the problem is nodes only know their addresses, not their index in vector
	 * but we can let them know while we push them into the vector
	 */
	int addNode(elementType data); //return index of the added node
	void removeNode(int nodeIndex); //how is it gonna work if we make dgNode private?
	void addLink(int fromNodeIndex, int toNodeIndex); //add a new link to "fromNode"
	void removeLink(int fromNodeIndex, int toNodeIndex); //remove a link from "fromNode"
	void printGraph(); //TODO: move this to << operator later
	void removeAllLinksTo(int nodeIndex); //for debug purposes, remove all links directed at a node
private:
	class dgNode {
	public:
		dgNode(elementType data);
		dgNode(elementType data, dgNode* adjNode);
		//we don't need a destructor, but we need a remove function which removes all links to the node in graph function!
		void addLink(dgNode* adjNode); //add new link to node at the end of links list
		void removeLink(dgNode* adjNode); //remove link to node from the links list
		void removeLinksToThis(); //remove all links coming to node from other nodes
		void removeLinksFromThis(); //remove all links going from this node and their references
		int getNumLinks();
		int getNumLinksFrom();
		elementType getData();
		friend ostream& operator<<(ostream& os, const digraph<elementType>::dgNode& node){
			digraph<elementType>::dgNode oNode = node;
			os << "Data: " << oNode.getData() << " @[" << &node << "]" << endl; //use &node to print original node addr
			os << "Has " << oNode.getNumLinks() <<" links to: " << *oNode.firstLinkTo << " " << endl;
			os << "Has " << oNode.getNumLinksFrom() << " links coming from: " << *oNode.topLinkFrom << endl;
		}; //print from firstNode to last
	private:
		class nodeLink { //linked list of nodes
		public:
			nodeLink(dgNode *adjNode, int weight=1); //create an unbound link
			nodeLink(dgNode *adjNode, nodeLink *previous, int weight=1); //create and place after *previous
			~nodeLink(); //set all forward indices -1
			nodeLink* removeLinkToNode(dgNode *adjNode); //find the link pointing to adjNode and remove it
			dgNode *getNode(); //return the node the link points to
			dgNode *getLink(int index); //give the pointer at index
			int getLastIndex(); //iterate elements one by one and return the last one's index
			int getIndex(); //TODO: make this inline!
			int getWeight(); //TODO: inline this...
			nodeLink *getNextLink(); //TODO: make this inline too.
			//must define << here, other places are "non-deducible context"
			friend ostream& operator<<(ostream& os, digraph<elementType>::dgNode::nodeLink& firstLink) {
				nodeLink *temp = &firstLink;
				while(temp) {
					os << temp->adjNode << "(" << temp->index << ")" << " ";
					temp = temp->nextLink;
				}
				os << temp; //print 0 at the end
			}; //print from firstNode to last

		private:
			dgNode *adjNode;
			nodeLink *nextLink;
			int index;
			int weight; //link weight
		};
		nodeLink *firstLinkTo; //pointer to first link in linked list of graph links from this node TO other nodes
		nodeLink *lastLinkTo; //just not to traverse whole list of links when we just need to check last one
		nodeLink *topLinkFrom; //a list of nodes who link to us. To make removal faster
		//man, I just need a stack of dgNode pointers for this :/
		//I'll only access the nodes which links to this sequentially when removing the node
		//just rename firstLinkFrom to topLinkFrom and it is a stack now :)

		elementType data;

	};
	list<dgNode> nodeList;
	vector<dgNode*> indexList; //holds pointers to nodes so that we can access them randomly via their addresses
};

//use it only for initial element of the linked list of links
template <typename elementType>
digraph<elementType>::dgNode::nodeLink::nodeLink(dgNode *adjNode, int weight) {
	this->adjNode = adjNode;
	this->nextLink = 0;
	this->index = 1;
	this->weight = weight;
}

/*
 * this function should only be used when previous != 0
 * or else previous remains 0 and we can't add any links behind it
 * these nodeLinks can't be used as a stack directly!
 */
template <typename elementType>
digraph<elementType>::dgNode::nodeLink::nodeLink(dgNode *adjNode, nodeLink *previous, int weight) {
	nodeLink *temp;
	this->adjNode = adjNode;
	weight = weight;
	if(previous==0) {
		cerr << "cannot add link, previous is non-defined" << endl;
		/*this->nextLink = 0;
		previous = this; //this doesn't mean anything, we get a reference to the previous pointer :( not the pointer itself
		this->index = 1;*/
	}
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
 * pointing previus link to the link after this one should be done first!
 * It is not the responsibility of destructor, it just adjusts the indices
 */
template <typename elementType>
digraph<elementType>::dgNode::nodeLink::~nodeLink() {
	nodeLink *temp = nextLink;
	while(temp) {
		temp->index--;
		temp=temp->nextLink;
	}
}

/*
 * the problem with this function is "what if we need to remove ourself?"
 * it returns 0 if the node we are trying to remove is "this" node,
 * unable to remove itself without severing the whole list
 * returns the link before the one we deleted otherwise
 */

template <typename elementType>
typename digraph<elementType>::dgNode::nodeLink* digraph<elementType>::dgNode::nodeLink::removeLinkToNode(dgNode *adjNode) {
	nodeLink *temp = this;
	nodeLink *remove;
	if(this->adjNode == adjNode) {
		return 0;
	}
	while(temp->nextLink) {
		if(temp->nextLink->adjNode == adjNode) {
			remove = temp->nextLink;
			temp->nextLink = remove->nextLink;
			delete remove; //reduce all forward indices
			return temp; //remove just one link and return its (old) index
		}
		else temp = temp->nextLink;
	}
	cerr << "Link to node " << adjNode << " cannot be found in list containing" << this << endl;
	//raise exception here!
}

/*
 * not sure if this function is useful in general.
 * returns a directed link from the linksTo list with index.
 * only useful for graphs in which order of links matters.
 */
//implementing nested templates comes with interesting grammar
//we have to state explicitly that digraph<elementType>::dgNode* is a typename by writing "typename" before it.
//Or else we get compiler error: error: need ‘typename’ before ‘digraph<elementType>::dgNode’ because ‘digraph<elementType>’ is a dependent scope
template <typename elementType>
typename digraph<elementType>::dgNode* digraph<elementType>::dgNode::nodeLink::getLink(int index) {
	nodeLink *temp = this;
	if(index<this->index) return 0;
	else {
		while(temp->index != index) {
			temp=temp->nextLink;
			if(temp == 0) break; //end of list
		}
	}
	return temp;
}

template <typename elementType>
typename digraph<elementType>::dgNode* digraph<elementType>::dgNode::nodeLink::getNode() {
	return this->adjNode;
}


template <typename elementType>
int digraph<elementType>::dgNode::nodeLink::getLastIndex() {
	nodeLink *temp;
	temp = this;
	if(temp != NULL) { //in case "this" points to 0, we don't want no segfaults
		while(temp->nextLink != 0) {
			temp = temp->nextLink;
		}
		return temp->index;
	}
	else return 0;
}

template <typename elementType>
int digraph<elementType>::dgNode::nodeLink::getIndex() {
	return this->index;
}

template <typename elementType>
int digraph<elementType>::dgNode::nodeLink::getWeight() {
	return this->weight;
}

template <typename elementType>
typename digraph<elementType>::dgNode::nodeLink* digraph<elementType>::dgNode::nodeLink::getNextLink() {
	return this->nextLink;
}

template <typename elementType>
digraph<elementType>::dgNode::dgNode(elementType data) { //init empty link list
	this->data = data;
	this->firstLinkTo = 0;
	this->lastLinkTo = 0;
	this->topLinkFrom = 0;
}

template <typename elementType>
digraph<elementType>::dgNode::dgNode(elementType data, dgNode *adjNode) { //init link list with first (and last) element = *adjnode
	this->data = data;
	firstLinkTo = new nodeLink(adjNode);
	lastLinkTo = firstLinkTo;
	this->topLinkFrom = 0;
}

template <typename elementType>
void digraph<elementType>::dgNode::addLink(dgNode *adjNode) { //add a link to adjNode at the end of link list
	if(lastLinkTo == 0) {
		lastLinkTo = new nodeLink(adjNode);
		firstLinkTo = lastLinkTo;
	}
	else lastLinkTo = new nodeLink(adjNode, lastLinkTo);
	if(adjNode->topLinkFrom == 0) adjNode->topLinkFrom = new nodeLink(this);
	else new nodeLink(this, adjNode->topLinkFrom); //constructor sets everything in this case
	//create a link from the node we are linking to this node too, to make removal of nodes easier
	//TODO: there may be bugs here due to the from stack/list, write examples and watch from stack/list
	//I guess I corrected them all
}

template <typename elementType>
void digraph<elementType>::dgNode::removeLink(dgNode *adjNode) {
	nodeLink *remove;
	nodeLink *previous;
	int lastIndex = lastLinkTo->getIndex();
	if(adjNode->topLinkFrom) { //let's check the reference links list on other side
		if(!adjNode->topLinkFrom->removeLinkToNode(this)) {
			//we need to remove first link ourself
			remove = adjNode->topLinkFrom;
			adjNode->topLinkFrom = remove->getNextLink(); //get the second link in the list
			delete remove; //destructor of remove adjusts the indices in whole list
		}
	}
	else cerr << "Can't find the reference to link in " << adjNode << " from " << this << ". Link reference list is empty!" << endl;

	if(this->firstLinkTo) {
		previous = this->firstLinkTo->removeLinkToNode(adjNode);
		if(!previous) {
			//again we have to remove the first link by hand
			remove = firstLinkTo;
			firstLinkTo = remove->getNextLink(); //make second link the first link
			delete remove; //get rid of the first link and adjust the indices
			if(firstLinkTo == 0) lastLinkTo = firstLinkTo; //all elements in the list have been removed
		}
		else if(previous->getNextLink()==0) lastLinkTo = previous; //we have deleted the end of the list and have to upgrade
	}
	else cerr << "There is no link to *" << adjNode << " to remove!" << endl;
	//we can change the return type and return false here.
	return;
}

template <typename elementType>
void digraph<elementType>::dgNode::removeLinksToThis() {
	typename dgNode::nodeLink *temp, *previous;
	//severe the links coming to the node
	//this part is the same as the second part of removeLink(adjNode) function
	//only it doesn't work on this node's list, it works on a list on another node (namely, othersideFirstLink)
	while(topLinkFrom) {
		previous = topLinkFrom->getNode()->firstLinkTo->removeLinkToNode(this);
		if(!previous) {
			temp = topLinkFrom->getNode()->firstLinkTo;
			topLinkFrom->getNode()->firstLinkTo = topLinkFrom->getNode()->firstLinkTo->getNextLink();
			delete temp; //delete the link on the other side
			if(topLinkFrom->getNode()->firstLinkTo == 0)  topLinkFrom->getNode()->lastLinkTo = topLinkFrom->getNode()->firstLinkTo;
		}
		else if(previous->getNextLink() == 0) topLinkFrom->getNode()->lastLinkTo = previous;
		temp = topLinkFrom;
		topLinkFrom = topLinkFrom->getNextLink(); //it is automatically zeroed at the end of stack/list
		delete temp; //delete the reference to link on the other side we just deleted
	}
	cerr << "I deleted all links to the node " << this << endl; //TODO: delete this later
}

template <typename elementType>
void digraph<elementType>::dgNode::removeLinksFromThis() {
	typename dgNode::nodeLink *temp = lastLinkTo;
	while(temp) {
		this->removeLink(temp->getNode());
		temp = lastLinkTo;
	}
	cerr << "I deleted all links from the node " << this << endl; //TODO: delete this later
}

template <typename elementType>
int digraph<elementType>::dgNode::getNumLinks() {
	if(lastLinkTo == 0) return 0;
	return lastLinkTo->getIndex();
}

template <typename elementType>
int digraph<elementType>::dgNode::getNumLinksFrom() { //get the number of links from other nodes to this node
	return this->topLinkFrom->getLastIndex();
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
 * Note that removing a node is always expensive:
 * 1) You meet each node that links to you, traverse their link lists and remove links to you
 * 2) node is removed from vector nodeList and all consequent elements are moved one element back
 */

template <typename elementType>
void digraph<elementType>::removeNode(int nodeIndex) {

	/*
	 * hard part is finding and removing all the links to this,
	 * makes you wish u used an adjacency matrix
	 * not complex but very expensive since
	 * we make a linear search through whole vector of nodes
	 * and a linear search through the links in those nodes
	 * every pointer in the whole data structure is accessed :/
	 * what if we used an adjacency matrix just for from links?
	 * or we can store the nodes we received links from in all nodes
	 */
	// yup, I did it! now we know all the nodes that has links toward the node we are going to remove

	//our links will be gone with us, no need to delete them!

	/*
	 * traverse the link references,
	 * meet every adjNode pointed by them and
	 * while they have links to us (the node to be removed)
	 * remove them with removeLink!
	 */
	/*typename dgNode::nodeLink *temp;
	dgNode *remove = nodeList.at(nodeIndex);
	//severe the links coming to the node
	while(remove->topLinkFrom) {
		remove->topLinkFrom->adjNode->removeLink(this);
		temp = remove->topLinkFrom;
		remove->topLinkFrom = remove->topLinkFrom->nextLink; //it is automatically zeroed at the end of stack/list
		delete temp;
	}
	cerr << "I deleted all links to the node " << this << endl; //TODO: delete this later
	*/

	//man upper comments became a mass :/
	typename list<dgNode>::iterator it = nodeList.begin();
	//it+=nodeIndex;
	advance(it,nodeIndex);
	//we need iterators here now that we are using a list to hold nodes
	(*it).removeLinksToThis();
	(*it).removeLinksFromThis();
	//delete all links in the node, more importantly delete their references in other nodes
	nodeList.erase(it);
	indexList[nodeIndex] = 0;

	//I hope this works :/

	return;
}

template <typename elementType>
void digraph<elementType>::addLink(int fromNodeIndex, int toNodeIndex) {
	typename list<dgNode>::iterator it1 = nodeList.begin(), it2 = nodeList.begin();
	//it1+=fromNodeIndex;
	advance(it1, fromNodeIndex);
	//it2+=toNodeIndex;
	advance(it2, toNodeIndex);
	//cout << "gonna add link from " << *it1 << " to " << *it2 << endl;
	(*it1).addLink(&(*it2));
	//fromNode->addLink(toNode);
	return;
}

template <typename elementType>
void digraph<elementType>::removeLink(int fromNodeIndex, int toNodeIndex) {
	typename list<dgNode>::iterator it1 = nodeList.begin(), it2 = nodeList.begin();
		//it1+=fromNodeIndex;
	advance(it1, fromNodeIndex);
	//it2+=toNodeIndex;
	advance(it2, toNodeIndex);
	(*it1).removeLink(&(*it2));
	//fromNode->removeLink(toNode);
	return;
}
/* This is so weird, I have to declare these functions inside the class or else it fails
 * They say "outside the class definition is a non-deducible context" on the net.
template <typename elementType>
ostream& operator<<(ostream& os, const typename digraph<elementType>::dgNode::nodeLink *firstLink) {
	typename digraph<elementType>::dgNode::nodeLink *temp = firstLink;
	while(temp) {
		os << temp->adjNode << "(" << temp->index << ")" << " ";
		temp = temp->nextLink;
	}
	os << temp; //print 0 at the end
}

template <typename elementType>
ostream& operator<<(ostream& os, const typename digraph<elementType>::dgNode *oNode) {
	os << "Data: " << oNode->getData << " @[" << oNode << "]" << endl;
	os << "Has " << oNode->getNumLinks() <<" links to: " << oNode->firstLinkTo << " " << endl;
	os << "Has " << oNode->getNumLinksFrom() << " links coming from: " << oNode->topLinkFrom << endl;
}
*/

/*
 * Print something like this:
 * nodenumber [address] : (number of links it sends, number of links it receives) link list
 */

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
		cout << "Node number: " << nodeNumber << " " << *it;
		nodeNumber++;
	}
	return;
}

template <typename elementType>
void digraph<elementType>::removeAllLinksTo(int nodeIndex) {

	typename list<dgNode>::iterator it = nodeList.begin();
	//it+=nodeIndex;
	advance(it,nodeIndex);
	(*it).removeLinksToThis();
	//nodeList.at(nodeIndex).removeLinksTo();
	//delete all links in the node, more importantly delete their references in other nodes
	//	nodeList.erase(nodeList.begin()+nodeIndex);
}




#endif /* DIGRAPH_H_ */
