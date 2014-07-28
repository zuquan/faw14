#include<algorithm>
#include<fstream>
#include"AdvancedDSTree.h"
#include"UnitTest.h"

extern vector<Y> allExistingY;
extern UnitTest * ut;

bool cmpY(Y y1, Y y2)
{
	return y1._y < y2._y;
}

// priority: increasing end, decreasing id
bool cmpX1(X x1, X x2)	//if x1.end == x2.end then (x1<x2 iff x1.id>x2.id)
{
	if (x1._end == x2._end)
	{
		return x1._id > x2._id;
	}
	else
	{
		return x1._end < x2._end;
	}
}

// priority: increasing end, increasing id
bool cmpX2(X x1, X x2) //if x1.end == x2.end then (x1.id<x2.id ===> x1<x2 )   
{
	if (x1._end == x2._end)
	{
		return x1._id < x2._id;
	}
	else
	{
		return x1._end < x2._end;
	}
}

// priority: increasing end, increasing begin, increasing id
bool cmpX3(X x1, X x2)
{
	if (x1._end == x2._end)
	{
		if (x1._begin == x2._begin)
		{
			return x1._id < x2._id;
		}
		else
		{
			return x1._begin < x2._begin;
		}
	}
	else
	{
		return x1._end < x2._end;
	}
}

// priority: decreasing begin, decreasing end, decreasing id
bool cmpXBegDec(X x1, X x2)
{
	if (x1._begin == x2._begin)
	{
		if (x1._end == x2._end)
		{
			return x1._id > x2._id;
		}
		else
		{
			return x1._end > x2._end;
		}
	}
	else
	{
		return x1._begin > x2._begin;
	}
}


// priority: weight-id order , increasing order
bool cmpXWeight(X x1, X x2)
{
	if (x1._w < x2._w)
	{
		return true;
	}
	else
	{
		if (x1._w == x2._w && x1._id < x2._id)
		{
			return true;
		}
	}
	return false;
}

// priority: begin order , increasing order
bool cmpXBegin(X x1, X x2)
{
	if (x1._begin < x2._begin)
	{
		return true;
	}
	else
	{
		if (x1._begin == x2._begin && x1._id < x2._id)
		{
			return true;
		}
	}
	return false;
}



ostream& operator<<(ostream& os, const Y& rhs)
{
	os << rhs._y;
	return os;
}

ostream& operator<<(ostream& os, const X& rhs)
{
	os << "X: " << rhs._id << " " << rhs._begin << " " << rhs._end << " " << rhs._w;
	return os;
}

AdvancedDSTreeNode::AdvancedDSTreeNode(vector<Y> allY)
{
	//cout << "create estree " << rangeOfY << endl;
	_pESTree = new ESTree(allY.size());
	_pEETree = new ESTree(allY.size());
	sort(allY.begin(), allY.end(), cmpY);
	for (int i = 0; i < (int)allY.size(); i++)
	{
		_values.push_back(allY[i]);
	}
	_leftChild = NULL;
	_rightChild = NULL;
	_parent = NULL;
}

AdvancedDSTree::AdvancedDSTree()
{
	_root = new AdvancedDSTreeNode(allExistingY);
}


Y AdvancedDSTreeNode::getIntervalStart()
{
	return _values[0];
}

AdvancedDSTreeNode* AdvancedDSTree::locateLeafOfX(X x)
{
	// assert: x's begin and end have been adjusted to existing value;
	AdvancedDSTreeNode* node = _root;
	while (x._begin._y != node->getIntervalStart()._y)
	{
		if (node->_rightChild != NULL)
		{
			if (x._begin._y >= ((AdvancedDSTreeNode*)node->_rightChild)->getIntervalStart()._y)
			{
				node = (AdvancedDSTreeNode*)node->_rightChild;
			}
			else
			{
				node = (AdvancedDSTreeNode*)node->_leftChild;
			}
		}
		else // assert there is no case that right child is NULL and left child is not NULL.
		{
			splitDSNode(node, x);
			updateAuxSet4Split(node);
			node = (AdvancedDSTreeNode*)node->_rightChild;
		}
	}
	while (node->_leftChild != NULL) // to leaf
	{
		node = node->_leftChild;
	}

	AdvancedDSTreeNode* tmp = node;
	while (tmp != NULL)
	{
		tmp->_variables.push_back(x);
		tmp = tmp->_parent;
	}
	return node;
}

void AdvancedDSTree::updateAuxSet4Split(AdvancedDSTreeNode* node)
{
	AdvancedDSTreeNode* leftChild = node->_leftChild;
	Msg msg;
	X tmpX;
	node->_matched.clear();
	node->_matched2.clear();
	node->_transferred.clear();
	node->_infeasible.clear();

	// for each variables in the previous node, reinsert into the left child
	for (int i = 0; i < (int)node->_variables.size(); i++)
	{
		tmpX = node->_variables[i];
		msg = leftChild->insertX(tmpX);		// insert into the left child
		switch (msg._c)
		{
		case 0:	// matched
		{
					node->_matched.push_back(tmpX);
		}break;
		case 1:	// transferred, add a into matched, delete b from matched, and add b into ES
		{
					node->_matched.push_back(tmpX);
					vector<X>::iterator it = find(node->_matched.begin(), node->_matched.end(), msg._b);
					node->_matched.erase(it);
					node->insertX(msg._b);
		}break;
		case 2:	// infeasible, add a into matched, remove b from matched and insert it into infeasible
		{
					node->_matched.push_back(tmpX);
					vector<X>::iterator it = find(node->_matched.begin(), node->_matched.end(), msg._b);
					node->_matched.erase(it);
					node->_infeasible.push_back(msg._b);
		}break;
		}
	}
}

void AdvancedDSTree::splitDSNode(AdvancedDSTreeNode* node, X x)
{
	vector<Y> leftVecY, rightVecY;
	int i = 0;
	while (node->_values[i]._y < x._begin._y)
	{
		leftVecY.push_back(node->_values[i]);
		i++;
	}
	while (i < (int)node->_values.size())
	{
		rightVecY.push_back(node->_values[i]);
		i++;
	}

	AdvancedDSTreeNode* leftChild = new AdvancedDSTreeNode(leftVecY);
	AdvancedDSTreeNode* rightChild = new AdvancedDSTreeNode(rightVecY);

	node->deleteCurrentESTree();
	node->_pESTree = new ESTree(rightVecY.size());
	node->_pEETree = new ESTree(rightVecY.size());

	leftChild->_variables = node->_variables;	// copy the variables from the parent

	node->_leftChild = leftChild;
	node->_rightChild = rightChild;
	leftChild->_parent = node;
	rightChild->_parent = node;
}

// adjust the begin and the end of an X to the existing values of Y;
// BEG[x] = celing(BEG[x])
// END[x] = floor(END[x])
bool AdvancedDSTree::adjustXToProper(X& x)
{

	int i = 0;
	double tempBegin;
	sort(allExistingY.begin(), allExistingY.end(), cmpY);
	while (x._begin._y > allExistingY[i]._y)
	{

		i++;
		if (i == allExistingY.size())
		{
			return false;
		}
	}
	tempBegin = allExistingY[i]._y;

	if (x._end._y < allExistingY[0]._y)
	{
		return false;
	}

	i = allExistingY.size() - 1;
	while (x._end._y < allExistingY[i]._y)
	{
		i--;
	}
	if (tempBegin > allExistingY[i]._y)
	{
		return false;
	}
	x._begin._y = tempBegin;
	x._end._y = allExistingY[i]._y;
	cout << x << endl;
	return true;
}

//Msg AdvancedDSTree::insertXOperation(X &x)


bool AdvancedDSTree::insertX(X &x)
{

	/*if (adjustXToProper(x) == false)
	{
	cout << x._id << " insert fail" << endl;
	return false;
	}*/
	AdvancedDSTreeNode* leaf = locateLeafOfX(x);
	Msg msg = leaf->insertX(x);		// insert the x into the leaf
	if (msg._c == 2)
	{
		msg._b = replaceMinWeightX(leaf, msg);		// call replaceable algorithm
		int a = 0;
	}

	while (leaf->_parent != NULL)	//send msg until the root, the msg is from a node to its parent
	{
		// leaf is the current node; the msg comes from its own ES-Tree insert operation
		if (leaf == leaf->_parent->_leftChild)	// the node is the left child
		{
			if (msg._aEmpty == false)
			{
				leaf->_parent->_matched.push_back(msg._a);
			}
			if (msg._bEmpty == false)
			{
				// questions: is it possible that the b is not in the matched set of the parent?
				vector<X>::iterator it = find(leaf->_parent->_matched.begin(), leaf->_parent->_matched.end(), msg._b);
				leaf->_parent->_matched.erase(it);
			}

			if (msg._c == 2)	// infeasible
			{
				leaf->_parent->_infeasible.push_back(msg._b);
			}
			else if (msg._c == 1)	// transferred
			{
				Msg tempMsg = leaf->_parent->insertX(msg._b);	// if tempMsg._b <> msg._b, then msg._b remains in the matched set of the parent
				msg._b = tempMsg._b;
				msg._bEmpty = tempMsg._bEmpty;
				msg._c = tempMsg._c;
				if (tempMsg._c == 2)
				{
					msg._b = replaceMinWeightX(leaf->_parent, tempMsg);		// call replaceable algorithm
					int a = 0;					
				}
			}
		}
		else // the current node is the right child
		{
			if (msg._bEmpty == false && msg._aEmpty == false && msg._b == msg._a)	//Fail in R
			{
				if (msg._c == 1)
				{
					leaf->_parent->_transferred.push_back(msg._a);
				}
				else
				{
					leaf->_parent->_infeasible.push_back(msg._a);
				}
			}
			else
			{
				Msg tempMsg = leaf->_parent->insertX(msg._a);

				sort(leaf->_parent->_matched.begin(), leaf->_parent->_matched.end(), cmpX3);
				vector<X>::iterator itTemp = find(leaf->_parent->_matched.begin(), leaf->_parent->_matched.end(), msg._b);

				// equvilant to check whether msg._b exists in the matched set of parent. If it is, then delete it.
				// if (tempMsg._bEmpty == false && msg._bEmpty == false && cmpX3(msg._b, tempMsg._b) == true)		//msg._b<tempMsg._b
				if (tempMsg._bEmpty == false && msg._bEmpty == false && itTemp != leaf->_parent->_matched.end())
				{
					leaf->_parent->removeX(msg);	// remove the msg._b from parent
					leaf->_parent->appendX(tempMsg);	// pull the tempMsg._b back
				}
				else if (tempMsg._bEmpty == true && msg._bEmpty != true)
				{
					leaf->_parent->removeX(msg);
				}
				else
				{
					msg._b = tempMsg._b;
					msg._bEmpty = tempMsg._bEmpty;
					msg._c = tempMsg._c;

					if (tempMsg._c == 2)
					{
						msg._b = replaceMinWeightX(leaf->_parent, tempMsg);		// call replaceable algorithm		

					}
				}

			}
		}
		leaf = leaf->_parent;
	}

	if (!(msg._bEmpty == false && msg._aEmpty == false && msg._b == msg._a))	// if a==b, do nothing
	{
		if (msg._aEmpty == false)	// a, which will be added, is not in matched before
		{
			_root->update.push_back(msg._a);
		}
		if (msg._bEmpty == false)	// b, which will be deleted, is in matched before
		{
			_root->update.push_back(msg._b);
		}
	}

	return true;
}

// call by parent in Brodal process
void AdvancedDSTreeNode::removeX(Msg m)
{
	_pESTree->deleteVariable(sizeOfY(_rightChild->_values[0], m._b._end));
	_pEETree->deleteVariable(_pEETree->allLeafNum() - sizeOfY(_rightChild->_values[0], m._b._begin));

	if (m._c == 1)
	{
		_transferred.push_back(m._b);
	}
	else if (m._c == 2)
	{
		_infeasible.push_back(m._b);
	}

	vector<X>::iterator it = find(_matched.begin(), _matched.end(), m._b);
	_matched.erase(it);	// delete b in the matched set of parent
	vector<X>::iterator it1 = find(_matched2.begin(), _matched2.end(), m._b);
	_matched2.erase(it1);	// delete b in the matched2 set of parent
}

// call by parent in Weight Matching case ::: Undo infeasible insertion; only change EETree, ESTree and Matched, Matched2 sets
void AdvancedDSTreeNode::removeXinWeightProcess(X x)
{
	const vector<Y>* pESValues;
	if (_rightChild != NULL)
	{
		pESValues = &_rightChild->_values;
	}
	else
	{
		pESValues = &_values;
	}
	_pESTree->deleteVariable(sizeOfY((*pESValues)[0], x._end));
	_pEETree->deleteVariable(_pEETree->allLeafNum() - sizeOfY((*pESValues)[0], x._begin));

	vector<X>::iterator it = find(_matched.begin(), _matched.end(), x);
	_matched.erase(it);	// delete b in the matched set of parent
	vector<X>::iterator it1 = find(_matched2.begin(), _matched2.end(), x);
	_matched2.erase(it1);	// delete b in the matched2 set of parent
}

void AdvancedDSTreeNode::appendX(Msg m)
{

	if (m._c == 1)
	{
		vector<X>::iterator it = find(_transferred.begin(), _transferred.end(), m._b);
		_transferred.erase(it);	// delete b in the matched set of parent
	}
	else if (m._c == 2)
	{
		vector<X>::iterator it = find(_infeasible.begin(), _infeasible.end(), m._b);
		_infeasible.erase(it);	// delete b in the matched set of parent
	}
	_pESTree->appendVariable(sizeOfY(_rightChild->_values[0], m._b._end));
	_pEETree->appendVariable(_pEETree->allLeafNum() - sizeOfY(_rightChild->_values[0], m._b._begin));
	_matched.push_back(m._b);
	_matched2.push_back(m._b);
}

// only change EETree, ESTree and Matched, Matched2 sets
void AdvancedDSTreeNode::appendXinWeightProcess(X x)
{
	const vector<Y>* pESValues;
	if (_rightChild != NULL)
	{
		pESValues = &_rightChild->_values;
	}
	else
	{
		pESValues = &_values;
	}

	_pESTree->appendVariable(sizeOfY((*pESValues)[0], x._end));
	_pEETree->appendVariable(_pEETree->allLeafNum() - sizeOfY((*pESValues)[0], x._begin));
	_matched.push_back(x);
	_matched2.push_back(x);
}


// compute the size between the start and the end; 
// Note: Y may not be integer.
int AdvancedDSTreeNode::sizeOfY(Y start, Y end)
{
	// assertion: start<=end && end<=max(Y)
	if (end < start)//in EETree
	{
		end = start;
	}
	if (start > end || end > allExistingY[allExistingY.size() - 1])
	{
		return 0;
	}

	int i = 0;
	int size = 0;
	while (i < (int)allExistingY.size() && end >= allExistingY[i])
	{
		if (start <= allExistingY[i])
		{
			size++;
		}
		i++;
	}
	return size;
}


void AdvancedDSTreeNode::deleteCurrentESTree(ESTreeNode* currentNode)
{
	if (currentNode == NULL)
	{
		return;
	}
	deleteCurrentESTree(currentNode->_rightChild);
	deleteCurrentESTree(currentNode->_leftChild);
	delete currentNode;
}

void AdvancedDSTreeNode::deleteCurrentESTree()
{
	deleteCurrentESTree(this->_pESTree->_root);
	deleteCurrentESTree(this->_pEETree->_root);
}


Msg AdvancedDSTreeNode::insertX(X x)
{
	Msg msg;
	ofstream fout("debug.txt", ios_base::in | ios_base::app);

	/*
	if (this->_values.size() == 23 && this->_values[0]._y == 8 && this->_values[22]._y == 30)
	{
	int a = 0;
	fout <<endl << "before test: x is: " << x << endl;
	ut->verifiyESTree(_pESTree->_root);
	if (x._id == 16)
	{
	int a = 0;
	}
	}*/

	vector<Y>* pESValues;
	if (_rightChild != NULL)
	{
		pESValues = &_rightChild->_values;
	}
	else
	{
		pESValues = &_values;
	}

	_matched.push_back(x);
	_matched2.push_back(x);

	// END[x]=23, kOfX=22 since ES-Tree values'=[2,30]
	int kOfX = sizeOfY((*pESValues)[0], x._end);	// the index of the END[x] in the current ESTree 

	int indexJ = _pESTree->insertVariable(kOfX);	//indexJ: 1...m+1
	if (indexJ <= _pESTree->allLeafNum() - 1)		//insert fail
	{
		Y endY;
		X preemptedX;
		endY = (*pESValues)[indexJ - 1];
		//preemptedX with id max
		if (endY == (*pESValues)[pESValues->size() - 1])
		{
			sort(_matched2.begin(), _matched2.end(), cmpX3);
			preemptedX = _matched2[_matched2.size() - 1];
			vector<X>::iterator it = find(_matched2.begin(), _matched2.end(), preemptedX);
			_matched2.erase(it);
			vector<X>::iterator it1 = find(_matched.begin(), _matched.end(), preemptedX);
			_matched.erase(it1);
		}
		else
		{
			sort(_matched2.begin(), _matched2.end(), cmpX3);	// fix the bug of replace (6,27), instead of (1,27)
			for (int i = (int)_matched2.size() - 1; i >= 0; i--)//assert: must can find an X with end endY
			{
				if (_matched2[i]._end == endY)
				{
					preemptedX = _matched2[i];
					vector<X>::iterator it = find(_matched2.begin(), _matched2.end(), preemptedX);
					_matched2.erase(it);
					vector<X>::iterator it1 = find(_matched.begin(), _matched.end(), preemptedX);
					_matched.erase(it1);
					break;
				}
			}
		}
		msg._a = x;
		msg._aEmpty = false;
		msg._b = preemptedX;
		msg._bEmpty = false;
		msg._c = (preemptedX._end > (*pESValues)[pESValues->size() - 1]) ? 1 : 2;

		if (preemptedX._end > (*pESValues)[pESValues->size() - 1])	// transferred
		{
			_transferred.push_back(preemptedX);
		}
		else // infeasible
		{
			_infeasible.push_back(preemptedX);
		}

	}
	else//success
	{
		msg._a = x;
		msg._aEmpty = false;
		msg._bEmpty = true;
		msg._c = 0;
	}

	//modify EETree
	if (msg._aEmpty == false)
	{
		_pEETree->appendVariable(_pEETree->allLeafNum() - sizeOfY((*pESValues)[0], msg._a._begin));
	}


	if (msg._bEmpty == false)
	{
		_pEETree->deleteVariable(_pEETree->allLeafNum() - sizeOfY((*pESValues)[0], msg._b._begin));
	}

	return msg;
}

bool AdvancedDSTree::isXMatched(X x)
{
	vector<X>::iterator tmp = find(_root->_matched.begin(), _root->_matched.end(), x);
	if (tmp == _root->_matched.end())	// not in matched set
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool AdvancedDSTree::isYMatched(Y y)
{
	AdvancedDSTreeNode *node = _root;

	while (node->_leftChild != NULL)
	{
		node->queryUpdateNodeW();
		if (node->_rightChild->_values[0] > y)
		{
			node = node->_leftChild;
		}
		else
		{
			node = node->_rightChild;
		}
	}
	node->queryUpdateLeafW();


	int idY = 0;
	for (idY = 0; idY < int(node->_values.size()); idY++)
	{
		if (node->_values[idY] == y)
		{
			idY++;
			break;
		}
	}
	if (idY <= int(node->_matching.size()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ASSERTION: isYMatched is run just before this one
X AdvancedDSTree::queryYMate(Y y)
{
	AdvancedDSTreeNode *node = _root;
	while (node->_leftChild != NULL)
	{
		if (node->_rightChild->_values[0] > y)
		{
			node = node->_leftChild;
		}
		else
		{
			node = node->_rightChild;
		}
	}

	return gloverMatchingInLeafForAnY(node, y);
}

Y AdvancedDSTree::queryXMate(X x)
{
	// assertion: x is matched.
	AdvancedDSTreeNode *node = _root;

	while (node->_leftChild != NULL)
	{
		node->queryUpdateNodeW();
		if (node->isXInLeftInQuery(x))
		{
			node = node->_leftChild;
		}
		else
		{
			node = node->_rightChild;
		}
	}
	node->queryUpdateLeafW();

	return gloverMatchingInLeafForAnX(node, x);
}

// update the W set of the current node, according to the current updateList set.
void AdvancedDSTreeNode::queryUpdateNodeW()
{
	vector<X>::iterator itUpdateList;
	vector<X> leftUpdateList, rightUpdateList;
	int leftSize = this->_leftChild->_values.size();

	for (itUpdateList = update.begin(); itUpdateList != update.end(); itUpdateList++)
	{
		if ((*itUpdateList)._id == 12)
		{
			int ad = 0;
		}
		vector<X>::iterator itMatching = find(_matching.begin(), _matching.end(), *itUpdateList);
		if (itMatching != _matching.end())	// in matching
		{
			vector<X>::iterator itW = find(lp.begin(), lp.end(), *itUpdateList);
			if (itW != lp.end())	// in W
			{
				if (isXInLeftInQuery(*itMatching))
				{
					leftUpdateList.push_back(*itMatching);
					if (int(lp.size()) > leftSize)	// W's size is greater than L.values
					{
						leftUpdateList.push_back(lp[leftSize]);		// the original leftSize^th one belongs to left child now.
						rightUpdateList.push_back(lp[leftSize]);	// dual of above
					}
				}
				else
				{
					rightUpdateList.push_back(*itMatching);
				}
				lp.erase(itW);
			}
			else
			{
				rightUpdateList.push_back(*itMatching);
			}
			_matching.erase(itMatching);
		}
		else // not in
		{
			vector<X>::iterator itMatched = find(this->_leftChild->_matched.begin(), this->_leftChild->_matched.end(), *itUpdateList);
			if (itUpdateList->_begin < this->_leftChild->_values[0] || itMatched != this->_leftChild->_matched.end())	// if this one should be added into W
			{
				lp.push_back(*itUpdateList);
				if (isXInLeftInQuery(*itUpdateList))
				{
					leftUpdateList.push_back(*itUpdateList);
					if (int(lp.size()) > leftSize)	// W's size is greater than L.values
					{
						leftUpdateList.push_back(lp[leftSize]);		// the orginal leftSize-1 one belongs to right child now
						rightUpdateList.push_back(lp[leftSize]);	// dual of above
					}
				}
				else
				{
					rightUpdateList.push_back(*itUpdateList);
				}
			}
			else
			{
				rightUpdateList.push_back(*itUpdateList);
			}
			_matching.push_back(*itUpdateList);
		}
	}

	update.clear();
	for (int i = 0; i < int(leftUpdateList.size()); i++)
	{
		this->_leftChild->update.push_back(leftUpdateList[i]);
	}
	for (int i = 0; i < int(rightUpdateList.size()); i++)
	{
		this->_rightChild->update.push_back(rightUpdateList[i]);
	}

}

bool AdvancedDSTreeNode::isXInLeftInQuery(X x)
{
	vector<X>::iterator tmp = find(lp.begin(), lp.end(), x);
	if (tmp == lp.end())	// not in W
	{
		return false;
	}

	// assertion: x \in W
	bool isLeft = false;
	int leftValues = 0;

	if (this->_leftChild->_values.size() > lp.size())
	{
		leftValues = lp.size();
	}
	else
	{
		leftValues = this->_leftChild->_values.size();
	}
	sort(lp.begin(), lp.end(), cmpX3);

	if (cmpX3(lp[leftValues - 1], x))
	{
		isLeft = false;
	}
	else
	{
		isLeft = true;
	}

	return isLeft;
}

void AdvancedDSTreeNode::queryUpdateLeafW()
{
	if (_leftChild != NULL)		// assertion: this is a leaf.
	{
		return;
	}

	for (vector<X>::iterator itL = update.begin(); itL != update.end(); itL++)
	{
		vector<X>::iterator itMatching = find(_matching.begin(), _matching.end(), *itL);
		if (itMatching != _matching.end())	// in matching
		{
			_matching.erase(itMatching);
		}
		else // not in
		{
			_matching.push_back(*itL);
		}
	}
	update.clear();
}

Y AdvancedDSTree::gloverMatchingInLeafForAnX(AdvancedDSTreeNode* leaf, X x)
{
	Y y;
	sort(leaf->_matching.begin(), leaf->_matching.end(), cmpX3);
	for (int i = 0; i < int(leaf->_matching.size()); i++)
	{
		if (leaf->_matching[i] == x)
		{
			y = leaf->_values[i];
			break;
		}
	}
	return y;
}

X AdvancedDSTree::gloverMatchingInLeafForAnY(AdvancedDSTreeNode* leaf, Y y)
{
	X x;
	sort(leaf->_matching.begin(), leaf->_matching.end(), cmpX3);
	for (int i = 0; i < int(leaf->_values.size()); i++)
	{
		if (leaf->_values[i] == y)
		{
			x = leaf->_matching[i];
			break;
		}
	}
	return x;
}

void AdvancedDSTree::verifiyDSTree(AdvancedDSTreeNode* root)
{
	cout << "debug format: (count, matched.size, transferred.size, infeasible.size)" << endl;
	deque<AdvancedDSTreeNode*> queue;
	queue.push_back(root);
	int count = 1;
	int level = 0;

	while (queue.empty() == false)
	{
		cout << "level" << level++ << ": ";
		int size = queue.size();
		for (int i = 0; i < size; i++)
		{
			AdvancedDSTreeNode* tmp = queue.front();
			queue.pop_front();
			if (tmp->_leftChild != NULL)
			{
				queue.push_back(tmp->_leftChild);
				queue.push_back(tmp->_rightChild);
			}
			cout << '(' << count++ << '|' << tmp->_matched.size() << '|' << tmp->_transferred.size() << '|' << tmp->_infeasible.size() << ')' << '\t';
		}
		cout << endl;
	}
}

void AdvancedDSTree::unitTestDS(string str)
{
	if (str == "DSTREE")
	{
		verifiyDSTree(_root);
	}
}


void AdvancedDSTree::replaceableSetOfP(AdvancedDSTreeNode* node, X x1, X jX, vector<X> &rset)
{
	rset.clear();

	sort(node->_matched2.begin(), node->_matched2.end(), cmpX3);
	vector<X>::iterator itL = node->_matched2.begin();

	while (itL != node->_matched2.end() && itL->_end <= jX._end)
	{
		rset.push_back(*itL);
		itL++;
	}
	rset.push_back(x1);	// add the inserted x iteself
}

void AdvancedDSTree::repalceableSetOfLeftChild(AdvancedDSTreeNode* node, X x, vector<X> &rset)
{
	rset.clear();

	vector<Y>* pEEValues;
	if (node->_rightChild != NULL)
	{
		pEEValues = &node->_rightChild->_values;
	}
	else
	{
		pEEValues = &node->_values;
	}

	int kOfX = node->sizeOfY((*pEEValues)[0], x._begin);
	int l = node->_pEETree->getLbyK(node->_pEETree->allLeafNum() - kOfX);	// m+1-k'
	sort(node->_matched2.begin(), node->_matched2.end(), cmpXBegDec);
	for (int i = 0; i < l; i++)
	{
		rset.push_back(node->_matched2[i]);
	}
	rset.push_back(x);	// add the inserted x iteself
}

X AdvancedDSTree::determineMinWeightX(AdvancedDSTreeNode* infeasibleNode, X newX, X jX, AdvancedDSTreeNode* &stopNode)
{
	stopNode = infeasibleNode;
	AdvancedDSTreeNode* curNode = infeasibleNode;
	X curMinWeightX;
	vector<X> curRInNode;
	// get curRInNode;
	replaceableSetOfP(infeasibleNode, newX, jX, curRInNode);
	sort(curRInNode.begin(), curRInNode.end(), cmpXWeight);
	curMinWeightX = curRInNode[0];
	sort(curRInNode.begin(), curRInNode.end(), cmpXBegin);

	vector<Y> esY = infeasibleNode->getESValues();
	if (curRInNode[0]._begin < esY[0])// there is at least a transferred X matchs 1-j
	{
		while (true)
		{
			curNode = curNode->_leftChild;
			//get curRInNode in the left child, provide the transferred X with min begin: curRInNode[0](parent's), curNode			
			repalceableSetOfLeftChild(curNode, curRInNode[0], curRInNode);
			sort(curRInNode.begin(), curRInNode.end(), cmpXWeight);
			if (cmpXWeight(curRInNode[0], curMinWeightX) == true)
			{
				curMinWeightX = curRInNode[0];
				stopNode = curNode;
			}
			if (continueCalToLeft(curNode, curRInNode) == true)
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}


	return curMinWeightX;
}

bool AdvancedDSTree::continueCalToLeft(AdvancedDSTreeNode* curNode, vector<X>& replaceableX)
{
	if (curNode->_leftChild == NULL)
	{
		return false;
	}
	else
	{
		sort(replaceableX.begin(), replaceableX.end(), cmpXBegin);
		if (replaceableX[0]._begin < curNode->getESValues()[0])
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}


vector<Y> AdvancedDSTreeNode::getESValues()
{
	if (_rightChild != NULL)
	{
		return _rightChild->_values;
	}
	else
	{
		return _values;
	}
}

// msg._a is the x vertex added into the node P
X AdvancedDSTree::replaceMinWeightX(AdvancedDSTreeNode* nodeP, Msg msg)
{
	//delete a from ESTree && EETree , add b back into ESEETree
	if (!(msg._a == msg._b))
	{
		nodeP->removeXinWeightProcess(msg._a);	// tempMsg._a is the original msg._b   //remove with out check end
		nodeP->appendXinWeightProcess(msg._b);  
	}
	vector<X>::iterator it = find(nodeP->_infeasible.begin(), nodeP->_infeasible.end(), msg._b);
	nodeP->_infeasible.erase(it);

	AdvancedDSTreeNode * stopNode = NULL;		// the node where the minX is in
	X minX = determineMinWeightX(nodeP, msg._a, msg._b, stopNode);

	if (stopNode == nodeP)
	{		
		if (!(minX == msg._a))
		{
			nodeP->removeXinWeightProcess(minX);
			nodeP->appendXinWeightProcess(msg._a);  //couldn't find in infeasible set
		}
		nodeP->_infeasible.push_back(minX);		// minX will be added into infeasbile nomatter minX == msg._a
	}
	else
	{
		// modify the stopNode		
		stopNode->_pESTree->deleteVariable(stopNode->sizeOfY(stopNode->getESValues()[0], minX._end));
		stopNode->_pEETree->deleteVariable(stopNode->_pEETree->allLeafNum() - stopNode->sizeOfY(stopNode->getESValues()[0], minX._begin));
		vector<X>::iterator it1 = find(stopNode->_matched2.begin(), stopNode->_matched2.end(), minX);
		stopNode->_matched2.erase(it1);	

		// modify the current mediate nodes, including the stopNode
		while (stopNode != nodeP)
		{
			stopNode = stopNode->pullBackATransferredXInWeightProcess(nodeP, minX, msg._a);
		}

		// add the (4, 12) back to P
		if (!(minX == msg._a))
		{
			// delete minX into infeasible
			vector<X>::iterator tmpIt = find(nodeP->_matched.begin(), nodeP->_matched.end(), minX);
			nodeP->_matched.erase(tmpIt);
			// add (4, 12), the inserted x which maybe transferred from a child
			nodeP->appendXinWeightProcess(msg._a);
		}
		nodeP->_infeasible.push_back(minX);		// minX will be added into infeasbile nomatter minX == msg._a
	}
	return minX;
}


AdvancedDSTreeNode* AdvancedDSTreeNode::pullBackATransferredXInWeightProcess(AdvancedDSTreeNode* infeasibleNode, X minWeightX, X insertedX)
{
	//return value
	AdvancedDSTreeNode* anc;
	// //for the first time, remember removeX minx from ESTreeEETree
	vector<X>::iterator minx = find(_matched.begin(), _matched.end(), minWeightX);
	_matched.erase(minx);
	_infeasible.push_back(minWeightX);

	//select
	vector<X> tm;

	for (unsigned int i = 0; i < _transferred.size(); i++)
	{
		X curX = _transferred[i];
		if (find(infeasibleNode->_matched.begin(), infeasibleNode->_matched.end(), curX) != infeasibleNode->_matched.end())
			//|| find(_parent->_transferred.begin(), _parent->_transferred.end(), curX) != _parent->_transferred.end())
			//if (find(_parent->_infeasible.begin(), _parent->_infeasible.end(), curX) == _parent->_infeasible.end())
		{
			//finally matched
			tm.push_back(curX);
		}
	}
	X backX;
	if (tm.size() == 0)
	{
		backX = insertedX;
	}
	else
	{
		sort(tm.begin(), tm.end(), cmpX3);
		backX = tm[0];
	}


	//add backX into current Node
	vector<X>::iterator it = find(_transferred.begin(), _transferred.end(), backX);
	_transferred.erase(it);

	_pESTree->appendVariable(sizeOfY(getESValues()[0], backX._end));
	_pEETree->appendVariable(_pEETree->allLeafNum() - sizeOfY(getESValues()[0], backX._begin));
	_matched.push_back(backX);
	_matched2.push_back(backX);

	//deal with the jumped DSNodes
	AdvancedDSTreeNode* curNode = this;
	while (true)
	{
		curNode = curNode->_parent;
		vector<X>::iterator it = find(curNode->_transferred.begin(), curNode->_transferred.end(), backX);
		if (it != curNode->_transferred.end())
		{
			//backX in transferred
			//pull back, no influence to ES or EE
			curNode->_transferred.erase(it);
			curNode->_matched.push_back(backX);
			//erase minWeightX
			vector<X>::iterator it2 = find(curNode->_matched.begin(), curNode->_matched.end(), minWeightX);
			curNode->_matched.erase(it2);
			curNode->_infeasible.push_back(minWeightX);
		}
		else
		{
			//not in transferred, //must in matched2
			//delete backX from matched2, keep in matched

			//backX may be the inserted x, which is not in the matched set
			if (!(backX == insertedX))
			{
				vector<X>::iterator it2 = find(curNode->_matched2.begin(), curNode->_matched2.end(), backX);
				curNode->_matched2.erase(it2);
				curNode->_pESTree->deleteVariable(sizeOfY(curNode->getESValues()[0], backX._end));
				curNode->_pEETree->deleteVariable(curNode->_pEETree->allLeafNum() - sizeOfY(curNode->getESValues()[0], backX._begin));

			}
			else
			{

			}

			anc = curNode;
			break;
		}
	}
	return anc;
}