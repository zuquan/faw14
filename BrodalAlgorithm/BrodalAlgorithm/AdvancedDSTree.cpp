#include<algorithm>
#include<fstream>
#include"AdvancedDSTree.h"
#include"UnitTest.h"

extern vector<Y> allExistingY;
extern UnitTest * ut;
extern ofstream olog;
extern AdvancedDSTree* debugTree;

bool cmpY(Y y1, Y y2)
{
	return y1._y < y2._y;
}

// priority: increasing id
bool cmpXID(X x1, X x2)
{
	return x1._id < x2._id;
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

// priority: increasing begin, increasing end, increasing id
bool cmpXBegInc(X x1, X x2)
{
	if (x1._begin == x2._begin)
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
	else
	{
		return x1._begin < x2._begin;
	}
}

// priority: reversed weight-id order , increasing order
// NOTE: change the id priority compared with ois version
bool cmpXWeight(X x1, X x2)
{
	if (x1._w == x2._w)
	{
		if (x1._end == x2._end)
		{
			if (x1._begin == x2._begin)
			{
				return x1._id > x2._id;
			}
			else
			{
				return x1._begin > x2._begin;
			}
		}
		else
		{
			return x1._end > x2._end;
		}
	}
	else
	{
		return x1._w < x2._w;
	}

	/*if (x1._w < x2._w)
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
	return false;*/
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
	_pEETree = NULL;	// there is NO EETree for a leaf
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

	vector<X> tmpMatched = node->_matched;
	node->_matched.clear();
	node->_matched2.clear();
	//node->_transferred.clear();
	//node->_infeasible.clear();

	leftChild->_transferred = node->_transferred;
	leftChild->_infeasible = node->_infeasible;		// TBC: may cause some problems in the process of vertex deletion

	// for each variables in the previous node, reinsert into the left child
	for (int i = 0; i < (int)tmpMatched.size(); i++)
	{
		tmpX = tmpMatched[i];
		msg = leftChild->insertX(tmpX);		// insert into the left child
		switch (msg._c)
		{
		case 0:	// matched
		{
					node->_matched.push_back(tmpX);
					node->_pEETree->appendVariable(node->_pEETree->allLeafNum() - node->sizeOfY(node->_values[0], tmpX._begin));
		}break;
		case 1:	// transferred, add a into matched, delete b from matched, and add b into ES
		{
					node->_matched.push_back(tmpX);
					node->_pEETree->appendVariable(node->_pEETree->allLeafNum() - node->sizeOfY(node->_values[0], tmpX._begin));
					vector<X>::iterator it = find(node->_matched.begin(), node->_matched.end(), msg._b);
					node->_matched.erase(it);
					node->_pEETree->deleteVariable(node->_pEETree->allLeafNum() - node->sizeOfY(node->_values[0], msg._b._begin));
					node->insertX(msg._b);
		}break;
		case 2:	// infeasible, add a into matched, remove b from matched and insert it into infeasible
		{
					vector<X> t; t.erase(t.begin());
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
	node->_pEETree = new ESTree(leftVecY.size());	// EETree for the left part of P
	node->_pESTree = new ESTree(rightVecY.size());

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

bool AdvancedDSTree::insertX(X &x)
{
	AdvancedDSTreeNode* nodeP = locateLeafOfX(x);
	Msg msg = nodeP->insertX(x);		// insert the x into the leaf
	if (msg._c == 2)
	{
		msg._b = replaceMinWeightX(nodeP, msg);		// call replaceable algorithm
	}
	AdvancedDSTreeNode* child = nodeP;
	nodeP = nodeP->_parent;

	while (nodeP != NULL)	//send msg until the root, the msg is from a child to its parent
	{
		if (child == nodeP->_leftChild)	// msg from the left child
		{
			if (msg._c == 0)	// 1.success in L
			{
				nodeP->_matched.push_back(msg._a);
				nodeP->_pEETree->appendVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], msg._a._begin));
			}
			else if (msg._a == msg._b)	// 2.non-sucessful, a = b
			{
				if (msg._c == 2)
				{
					nodeP->_infeasible.push_back(msg._b);
				}
				else
				{
					Msg tempMsg = nodeP->insertX(msg._b);
					if (tempMsg._c == 2)
					{
						tempMsg._b = replaceMinWeightX(nodeP, tempMsg);		// call replacement algorithm
					}
					msg._b = tempMsg._b;
					msg._bEmpty = tempMsg._bEmpty;
					msg._c = tempMsg._c;
				}
			}
			else   // 3.non-sucessful, a != b
			{
				vector<X>::iterator it = find(nodeP->_matched.begin(), nodeP->_matched.end(), msg._b);
				if (it == nodeP->_matched.end())	// b is preempted by vertex from R and is NOT in M(P) now.
				{
					msg._b = replaceMinWeightXFromLeft(nodeP, msg);		// call replacement algorithm from left
					msg._c = 2;
					if (msg._b._end > nodeP->_values[nodeP->_values.size() - 1])
					{
						vector<X> t; t.erase(t.begin());
					}
				}
				else    // replace b with a in the left of P, try to insert b if trans
				{
					if (msg._c == 2)	// infeasible
					{
						vector<X>::iterator it = find(nodeP->_matched.begin(), nodeP->_matched.end(), msg._b);
						nodeP->_matched.erase(it);
						nodeP->_pEETree->deleteVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], msg._b._begin));
						nodeP->_infeasible.push_back(msg._b);
						nodeP->_matched.push_back(msg._a);
						nodeP->_pEETree->appendVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], msg._a._begin));
					}
					else    // transfer
					{
						// get the transfer x of maximum
						vector<X> lRSet = getLeftReplaceableSetOfP(nodeP, msg._a);
						lRSet.push_back(msg._a);
						sort(lRSet.begin(), lRSet.end(), cmpX3);
						if (lRSet[lRSet.size() - 1] != msg._b)
						{
							msg._b = lRSet[lRSet.size() - 1];
						}

						// update the left part of P
						vector<X>::iterator it = find(nodeP->_matched.begin(), nodeP->_matched.end(), msg._b);
						nodeP->_matched.erase(it);
						nodeP->_pEETree->deleteVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], msg._b._begin));
						nodeP->_matched.push_back(msg._a);
						nodeP->_pEETree->appendVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], msg._a._begin));

						// insert transferred x in the right of P
						Msg tempMsg = nodeP->insertX(msg._b);
						if (tempMsg._c == 2)
						{
							tempMsg._b = replaceMinWeightX(nodeP, tempMsg);		// call replacement algorithm
						}
						msg._b = tempMsg._b;
						msg._bEmpty = tempMsg._bEmpty;
						msg._c = tempMsg._c;
					}
				}
				// end of 3.non-sucessful, a != b
			}
			// end of case of msg from left child 
		}
		else // msg from the right child
		{
			if (msg._bEmpty == false && msg._aEmpty == false && msg._b == msg._a)	// Fail in R and kick itself out
			{
				if (msg._c == 1)
				{
					nodeP->_transferred.push_back(msg._a);
				}
				else
				{
					nodeP->_infeasible.push_back(msg._a);
				}
			}
			else if (msg._c != 0 && find(nodeP->_matched.begin(), nodeP->_matched.end(), msg._b) != nodeP->_matched.end())
			{
				// the replacement set R(x) does not change in P; remove minx in P and msg remain the same
				nodeP->removeX(msg);	// remove the msg._b
				// add msg._a
				nodeP->_pESTree->appendVariable(nodeP->sizeOfY(nodeP->_rightChild->_values[0], msg._a._end));
				nodeP->_matched.push_back(msg._a);
				nodeP->_matched2.push_back(msg._a);
			}
			else
			{
				Msg tempMsg = nodeP->insertX(msg._a);
				if (tempMsg._c == 2)
				{
					tempMsg._b = replaceMinWeightX(nodeP, tempMsg);		// call replacement algorithm
				}
				msg = tempMsg;	// since msg._a = tempMsg._a

				if (tempMsg._c == 0 && msg._c != 0)
				{
					vector<X> t; t.erase(t.begin());
				}
			}
		}
		child = nodeP;
		nodeP = nodeP->_parent;
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
			olog << "insert id:" << msg._a._id << " replace id:" << msg._b._id << endl;
		}
		else
		{
			olog << "insert id:" << msg._a._id << " success" << endl;
		}
	}
	else
	{
		olog << "insert id:" << msg._a._id << " fail" << endl;
	}

	return true;
}

// call by parent
void AdvancedDSTreeNode::removeX(Msg m)
{
	_pESTree->deleteVariable(sizeOfY(_rightChild->_values[0], m._b._end));

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

// call by parent in Weight Matching case ::: Undo infeasible insertion; change EETree and Matched, Matched2 sets
void AdvancedDSTreeNode::removeXinRightOfP(X x)
{
	_pESTree->deleteVariable(sizeOfY(getESValues()[0], x._end));

	vector<X>::iterator it = find(_matched.begin(), _matched.end(), x);
	_matched.erase(it);
	vector<X>::iterator it1 = find(_matched2.begin(), _matched2.end(), x);
	_matched2.erase(it1);

	_infeasible.push_back(x);
}

// remove x in the left of P by updating EETree and Matched, Infeasible
void AdvancedDSTreeNode::removeXinLeftOfP(X x)
{
	_pEETree->deleteVariable(_pEETree->allLeafNum() - sizeOfY(_values[0], x._begin));
	vector<X>::iterator it = find(_matched.begin(), _matched.end(), x);
	_matched.erase(it);
	_infeasible.push_back(x);
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
	_matched.push_back(m._b);
	_matched2.push_back(m._b);
}

// change ESTree and Matched, Matched2 sets, have NOT change infeasible set
void AdvancedDSTreeNode::appendXinRightOfP(X x)
{
	_pESTree->appendVariable(sizeOfY(getESValues()[0], x._end));
	_matched.push_back(x);
	_matched2.push_back(x);
}

// move transferred matched x from right to Left of P, updating EETree, ESTree and Matched2 set
void AdvancedDSTreeNode::moveXFromRight2Left(X x, bool isInsertedX)
{
	if (isInsertedX == false)	// if not insertedX
	{
		_pESTree->deleteVariable(sizeOfY(getESValues()[0], x._end));
		vector<X>::iterator it1 = find(_matched2.begin(), _matched2.end(), x);
		_matched2.erase(it1);
		_pEETree->appendVariable(_pEETree->allLeafNum() - sizeOfY(_values[0], x._begin));
	}
	else
	{
		_pEETree->appendVariable(_pEETree->allLeafNum() - sizeOfY(_values[0], x._begin));
		_matched.push_back(x);
	}
}

// move transferred matched x from left to right of P, updating EETree, ESTree and Matched2, matched set
void AdvancedDSTreeNode::moveXFromLeft2Right(X x, bool isInsertedX)
{
	if (isInsertedX == false)	// if not insertedX
	{
		_pEETree->deleteVariable(_pEETree->allLeafNum() - sizeOfY(_values[0], x._begin));
		_matched2.push_back(x);
		_pESTree->appendVariable(sizeOfY(getESValues()[0], x._end));
	}
	else
	{
		_matched.push_back(x);
		_matched2.push_back(x);
		_pESTree->appendVariable(sizeOfY(getESValues()[0], x._end));
	}
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
	if (this->_pEETree != NULL)
	{
		deleteCurrentESTree(this->_pEETree->_root);
	}
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
		/*if ((*itUpdateList)._id == 12)
		{
		int ad = 0;
		}*/
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

// get transferred matched set
vector<X> AdvancedDSTree::getTransMatchedSet(AdvancedDSTreeNode* node, vector<X> &rset)
{
	vector<X> tMset;

	sort(rset.begin(), rset.end(), cmpXBegInc);
	int i = 0;
	while (i < rset.size() && rset[i]._begin < node->getESValues()[0])
	{
		tMset.push_back(rset[i]);
		i++;
	}

	return tMset;
}

// return the replacement set in the left part of P. x is the inserted vertex.
// NOTE: the result set does not contain the reference vertex x.
vector<X> AdvancedDSTree::getLeftReplaceableSetOfP(AdvancedDSTreeNode* node, X x)
{
	vector<X> leftSet;
	sort(node->_matched.begin(), node->_matched.end(), cmpXID);
	sort(node->_matched2.begin(), node->_matched2.end(), cmpXID);
	int i = 0;
	int j = 0;
	while (i < node->_matched.size() && j < node->_matched2.size())
	{
		if (node->_matched[i] != node->_matched2[j])
		{
			leftSet.push_back(node->_matched[i]);
			i++;
		}
		else
		{
			i++;
			j++;
		}
	}
	if (i < node->_matched.size())
	{
		while (i < node->_matched.size())
		{
			leftSet.push_back(node->_matched[i]);
			i++;
		}
	}

	vector<X> rset;
	int kOfX = node->sizeOfY(node->_values[0], x._begin);
	int l = node->_pEETree->getLbyK(node->_pEETree->allLeafNum() - kOfX);	// m+1-k'
	if (l > leftSet.size())
	{
		l = leftSet.size();
	}

	sort(leftSet.begin(), leftSet.end(), cmpXBegDec);
	// the tight point l means there are l x vertices should be picked
	for (int i = 0; i < l; i++)
	{
		rset.push_back(leftSet[i]);
	}
	
	return rset;
}

// return the replacement set in the left part of P. x is the inserted vertex.
// NOTE: does not contain the reference x.
vector<X> AdvancedDSTree::getRihgtReplaceableSetOfP(AdvancedDSTreeNode* node, X x)
{
	vector<X> rset;
	int kOfX = node->sizeOfY(node->getESValues()[0], x._end);
	int j = node->_pESTree->getLbyK(kOfX);	// a_j=j

	sort(node->_matched2.begin(), node->_matched2.end(), cmpX3);
	if (j > node->_matched2.size())
	{
		vector<X> t; t.erase(t.begin());
	}

	for (int i = 0; i < j; i++)
	{
		rset.push_back(node->_matched2[i]);
	}

	return rset;
}

// get the values of ESTree, i.e., the values of the right child
const vector<Y>& AdvancedDSTreeNode::getESValues()
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

// get the set which can replace minX and pick the one with maximum end
X AdvancedDSTreeNode::getBackX(vector<X> transMSet, X minX)
{
	Y refY;
	if (minX._begin == _values[_values.size() - _rightChild->_values.size() - 1])
	{
		refY = _rightChild->_values[0];
	}
	else
	{
		int kOfX = sizeOfY(_values[0], minX._begin);
		int l2 = _pEETree->get2ndLbyK(_pEETree->allLeafNum() - kOfX);	// m+1-k'
		int p = _values.size() - _rightChild->_values.size() - l2 + 1;

		if (l2 == 0)
		{
			refY = _rightChild->_values[0];
		}
		else
		{
			refY = _values[p - 1];
		}
	}

	vector<X> refS;
	for (int i = 0; i < transMSet.size(); i++)
	{
		if (transMSet[i]._begin < refY)
		{
			refS.push_back(transMSet[i]);
		}
	}

	if (refS.empty())
	{
		vector<X> t; t.erase(t.begin());
	}
	sort(refS.begin(), refS.end(), cmpX3);
	return refS[0];
}

// insertedX is the x vertex added into the node P
X AdvancedDSTree::replaceMinWeightX(AdvancedDSTreeNode* nodeP, Msg msg)
{
	X insertedX = msg._a;
	X removedX = msg._b;

	// roll back the insert in P
	if (!(insertedX == removedX))
	{
		// roll back the insertion of insertedX
		nodeP->_pESTree->deleteVariable(nodeP->sizeOfY(nodeP->getESValues()[0], insertedX._end));
		vector<X>::iterator it = find(nodeP->_matched.begin(), nodeP->_matched.end(), insertedX);
		nodeP->_matched.erase(it);
		vector<X>::iterator it1 = find(nodeP->_matched2.begin(), nodeP->_matched2.end(), insertedX);
		nodeP->_matched2.erase(it1);

		// roll back the deletion of removedX
		nodeP->_pESTree->appendVariable(nodeP->sizeOfY(nodeP->getESValues()[0], removedX._end));
		nodeP->_matched.push_back(removedX);
		nodeP->_matched2.push_back(removedX);
	}
	vector<X>::iterator it = find(nodeP->_infeasible.begin(), nodeP->_infeasible.end(), removedX);
	nodeP->_infeasible.erase(it);		// no matter insertedX == removedX or not

	// compute the replacement set
	X minX = insertedX;
	bool inLeft = false;
	vector<X> rightRSet, leftRSet, transMSet;	// replacement set	

	// update insertedX if it can preempted an x in the left with larger end
	if (insertedX._begin < nodeP->getESValues()[0])
	{
		vector<X> tmpleftRSet = getLeftReplaceableSetOfP(nodeP, insertedX);
		sort(tmpleftRSet.begin(), tmpleftRSet.end(), cmpX3);

		if (cmpX3(insertedX, tmpleftRSet[tmpleftRSet.size() - 1]))
		{
			X forwardX = tmpleftRSet[tmpleftRSet.size() - 1];
			// exchange tmpX and insertedX
			vector<X>::iterator it = find(nodeP->_matched.begin(), nodeP->_matched.end(), forwardX);
			nodeP->_matched.erase(it);
			nodeP->_pEETree->deleteVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], forwardX._begin));

			nodeP->_matched.push_back(insertedX);
			nodeP->_pEETree->appendVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], insertedX._begin));

			insertedX = forwardX;
			minX = insertedX;
		}
	}

	// get rightRset and TM set
	rightRSet = getRihgtReplaceableSetOfP(nodeP, insertedX);
	transMSet = getTransMatchedSet(nodeP, rightRSet);
	if (insertedX._begin < nodeP->getESValues()[0])
	{
		transMSet.push_back(insertedX);
	}

	// get leftRset and update rRset if necessary
	if (transMSet.empty() == false)		// there is at least a transferred X matchs 1-j
	{
		// get the R(x) by EE-Tree
		sort(transMSet.begin(), transMSet.end(), cmpXBegInc);	// increasing x.begin, get x with min begin
		X refBackX = transMSet[0];
		leftRSet = getLeftReplaceableSetOfP(nodeP, refBackX);

		sort(leftRSet.begin(), leftRSet.end(), cmpX3);
		sort(rightRSet.begin(), rightRSet.end(), cmpX3);
		if (leftRSet[leftRSet.size() - 1]._end > rightRSet[rightRSet.size() - 1]._end)
		{
			rightRSet = getRihgtReplaceableSetOfP(nodeP, leftRSet[leftRSet.size() - 1]);
			transMSet = getTransMatchedSet(nodeP, rightRSet);
			if (insertedX._begin < nodeP->getESValues()[0])
			{
				transMSet.push_back(insertedX);
			}

			// may be there is another more extension
			sort(rightRSet.begin(), rightRSet.end(), cmpXBegInc);
			sort(leftRSet.begin(), leftRSet.end(), cmpXBegInc);
			if (rightRSet[0]._begin < leftRSet[0]._begin)
			{
				leftRSet = getLeftReplaceableSetOfP(nodeP, rightRSet[0]);
			}
		}

		//for test	// assertion, there is no any more extension
		{
			sort(leftRSet.begin(), leftRSet.end(), cmpX3);
			sort(rightRSet.begin(), rightRSet.end(), cmpX3);
			if (leftRSet[leftRSet.size() - 1]._end > rightRSet[rightRSet.size() - 1]._end)
			{
				vector<X> t; t.erase(t.begin());
			}
		}
		//end of test

	}

	// get minX
	sort(rightRSet.begin(), rightRSet.end(), cmpXWeight);
	if (cmpXWeight(rightRSet[0], minX))
	{
		minX = rightRSet[0];
		inLeft = false;
	}
	if (leftRSet.empty() == false)
	{
		sort(leftRSet.begin(), leftRSet.end(), cmpXWeight);
		if (cmpXWeight(leftRSet[0], minX))
		{
			minX = leftRSet[0];
			inLeft = true;
		}
	}

	// update auxilary trees and sets
	if (minX == insertedX)
	{
		nodeP->_infeasible.push_back(minX);
		return minX;
	}

	if (inLeft)
	{
		X backX = nodeP->getBackX(transMSet, minX);		// get the backX using the get2ndLbyK

		nodeP->removeXinLeftOfP(minX);	// remove minX
		bool isInsertedX = (backX == insertedX);
		nodeP->moveXFromRight2Left(backX, isInsertedX);	// move from right to left the transferred matched vertex x having min{x.end}

		if (isInsertedX == false)
		{
			nodeP->appendXinRightOfP(insertedX);	// add insertedX
		}
	}
	else
	{
		nodeP->removeXinRightOfP(minX);		// remove minX
		nodeP->appendXinRightOfP(insertedX);	// add insertedX
	}

	return minX;
}

// find the repalcement set from the left of P, msg._b is NOT in M(P)
X AdvancedDSTree::replaceMinWeightXFromLeft(AdvancedDSTreeNode* nodeP, Msg msg)
{
	X insertedX = msg._a;
	vector<X> leftRSet, rightRSet, transMSet;
	X minX = insertedX;
	bool inLeft = true;
	X forwardX;

	// get left replacement set
	leftRSet = getLeftReplaceableSetOfP(nodeP, insertedX);
	sort(leftRSet.begin(), leftRSet.end(), cmpX3);
	forwardX = leftRSet[leftRSet.size() - 1];

	// update insertedX if it can preempted an x in the left with larger end
	if (cmpX3(insertedX, forwardX))		// if insertedX < forwardX
	{	
		vector<X>::iterator it = find(nodeP->_matched.begin(), nodeP->_matched.end(), forwardX);
		nodeP->_matched.erase(it);
		nodeP->_pEETree->deleteVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], forwardX._begin));

		nodeP->_matched.push_back(insertedX);
		nodeP->_pEETree->appendVariable(nodeP->_pEETree->allLeafNum() - nodeP->sizeOfY(nodeP->_values[0], insertedX._begin));

		it = find(leftRSet.begin(), leftRSet.end(), forwardX);
		leftRSet.erase(it);
		leftRSet.push_back(insertedX);

		insertedX = forwardX;
		minX = insertedX;
	}

	// get right replacement set
	if (insertedX._end < nodeP->getESValues()[0])
	{
		vector<X> t; t.erase(t.begin());
	}
	rightRSet = getRihgtReplaceableSetOfP(nodeP, insertedX);
	transMSet = getTransMatchedSet(nodeP, rightRSet);
	if (insertedX._begin < nodeP->getESValues()[0])
	{
		transMSet.push_back(insertedX);
	}

	// change whether there is a x of right has smaller begin
	sort(rightRSet.begin(), rightRSet.end(), cmpXBegInc);
	sort(leftRSet.begin(), leftRSet.end(), cmpXBegInc);
	if (rightRSet[0]._begin < leftRSet[0]._begin)
	{
		leftRSet = getLeftReplaceableSetOfP(nodeP, rightRSet[0]);
	}

	//for test	// assertion, there is no any more extension
	{
		sort(leftRSet.begin(), leftRSet.end(), cmpX3);
		sort(rightRSet.begin(), rightRSet.end(), cmpX3);
		if (leftRSet[leftRSet.size() - 1]._end > rightRSet[rightRSet.size() - 1]._end)
		{
			vector<X> t; t.erase(t.begin());
		}
	}
	//end of test

	// get minX
	sort(leftRSet.begin(), leftRSet.end(), cmpXWeight);
	if (cmpXWeight(leftRSet[0], minX))
	{
		minX = leftRSet[0];
		inLeft = true;
	}
	if (rightRSet.empty() == false)
	{
		sort(rightRSet.begin(), rightRSet.end(), cmpXWeight);
		if (cmpXWeight(rightRSet[0], minX))
		{
			minX = rightRSet[0];
			inLeft = false;
		}
	}

	// update auxilary trees and sets
	if (minX == insertedX)
	{
		nodeP->_infeasible.push_back(minX);
		return minX;
	}

	if (inLeft)
	{
		X backX = nodeP->getBackX(transMSet, minX);		// get the backX using the get2ndLbyK

		nodeP->removeXinLeftOfP(minX);	// remove minX
		bool isInsertedX = (backX == insertedX);
		nodeP->moveXFromRight2Left(backX, isInsertedX);	// move from right to left the transferred matched vertex x having min{x.end}

		if (isInsertedX == false)
		{
			nodeP->appendXinRightOfP(insertedX);	// add insertedX
		}
	}
	else
	{
		nodeP->removeXinRightOfP(minX);		// remove minX
		nodeP->appendXinRightOfP(insertedX);	// add insertedX
	}

	return minX;
}
