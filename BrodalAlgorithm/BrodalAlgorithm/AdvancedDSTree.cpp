#include<algorithm>
#include"AdvancedDSTree.h"


extern vector<Y> allExistingY;

bool cmpY(Y y1, Y y2)
{
	return y1._y < y2._y;
}

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


//1=>id da de zai qian
//2=>id xiao de zai qian

ostream& operator<<(ostream& os, const Y& rhs)
{
	os << rhs._y;
	return os;
}

ostream& operator<<(ostream& os, const X& rhs)
{
	os << "X: " << rhs._id << " " << rhs._begin << " " << rhs._end;
	return os;
}

AdvancedDSTreeNode::AdvancedDSTreeNode(vector<Y> allY)
{
	//cout << "create estree " << rangeOfY << endl;
	_pESTree = new ESTree(allY.size());
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
	node->_transferred.clear();
	node->_infeasible.clear();
	for (int i = 0; i < (int)node->_variables.size(); i++)
	{
		tmpX = node->_variables[i];
		msg = leftChild->insertX(tmpX);
		switch (msg._c)
		{
		case 0:	// matched
		{
					node->_matched.push_back(tmpX);
		}break;
		case 1:	// transferred
		{
					node->insertX(tmpX);
		}break;
		case 2:	// infeasible
		{
					node->_infeasible.push_back(tmpX);
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

	if (adjustXToProper(x) == false)
	{
		cout << x._id << " insert fail" << endl;
		return false;
	}
	AdvancedDSTreeNode* leaf = locateLeafOfX(x);
	Msg finalMsg;
	Msg msg = leaf->insertX(x);
	while (leaf->_parent != NULL)//send msg
	{
		if (leaf == leaf->_parent->_leftChild)
		{
			if (msg._aEmpty == false)
			{
				leaf->_parent->_matched.push_back(msg._a);
			}
			if (msg._bEmpty == false)
			{
				vector<X>::iterator it = find(leaf->_parent->_matched.begin(), leaf->_parent->_matched.end(), msg._b);
				leaf->_parent->_matched.erase(it);
			}
			if (msg._c == 2)
			{
				leaf->_parent->_infeasible.push_back(msg._b);
			}
			else if (msg._c == 1)
			{
				Msg tempMsg = leaf->_parent->insertX(msg._b);
				msg._b = tempMsg._b;
				msg._bEmpty = tempMsg._bEmpty;
				msg._c = tempMsg._c;
			}
		}
		else
		{

			if (msg._bEmpty == false && msg._aEmpty == false && msg._b == msg._a)//Fail in R
			{
				/*msg._aEmpty = false;
				msg._bEmpty = false;
				msg._a = tempMsg._a;
				msg._b = tempMsg._a;
				msg._c = tempMsg._c;*/
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
				if (tempMsg._bEmpty == true && msg._bEmpty != true)
				{
					leaf->_parent->_pESTree->deleteTheX(leaf->_parent->sizeOfY(leaf->_values[0], msg._b._end));
					if (msg._c == 1)
					{
						leaf->_parent->_transferred.push_back(msg._b);
					}
					else
					{
						leaf->_parent->_infeasible.push_back(msg._b);
					}
					vector<X>::iterator it = find(leaf->_parent->_matched.begin(), leaf->_parent->_matched.end(), msg._b);
					leaf->_parent->_matched.erase(it);
				}
				else
				{
					msg._b = tempMsg._b;
					msg._bEmpty = tempMsg._bEmpty;
					msg._c = tempMsg._c;
				}

			}
		}
		leaf = leaf->_parent;
	}

	if (msg._aEmpty == false)
	{
		_root->update.push_back(msg._a);
	}
	if (msg._bEmpty == false)
	{
		_root->update.push_back(msg._b);
	}


	return true;
}

// compute the size between the start and the end; 
// Note: Y may not be integer.
int AdvancedDSTreeNode::sizeOfY(Y start, Y end)
{
	// assertion: start<=end && end<=max(Y)
	if (start > end || end > allExistingY[allExistingY.size() - 1])
		return 0;
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
}


Msg AdvancedDSTreeNode::insertX(X x)
{
	Msg msg;

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

	int kOfX = sizeOfY((*pESValues)[0], x._end);
	int indexJ = _pESTree->insertVariable(kOfX);//indexJ: 1...m+1
	if (indexJ <= _pESTree->allLeafNum() - 1)//insert fail
	{
		Y endY;
		X preemptedX;
		endY = (*pESValues)[indexJ - 1];
		//preemptedX with id max
		if (endY == (*pESValues)[pESValues->size() - 1])
		{
			sort(_matched.begin(), _matched.end(), cmpX2);
			preemptedX = _matched[_matched.size() - 1];
			vector<X>::iterator it = find(_matched.begin(), _matched.end(), preemptedX);
			_matched.erase(it);
		}
		else
		{
			sort(_matched.begin(), _matched.end(), cmpX1);
			for (int i = 0; i < (int)_matched.size(); i++)//assert: must can find an X with end endY
			{
				if (_matched[i]._end == endY)
				{
					preemptedX = _matched[i];
					vector<X>::iterator it = find(_matched.begin(), _matched.end(), preemptedX);
					_matched.erase(it);
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

Y AdvancedDSTree::queryXMate(X x)
{
	// To Be Checked, move to outside
	//_root->_matching = _root->_matched;

	//vector<X>::iterator itL;
	//for (itL = _root->_matching.begin(); itL != _root->_matching.end(); itL++)
	//{
	//	vector<X>::iterator itV = find(_root->_leftChild->_matched.begin(), _root->_leftChild->_matched.end(), *itL);
	//	if (itL->_begin < _root->_leftChild->_values[0] || itV != _root->_leftChild->_matched.end())	// not in W before
	//	{
	//		_root->lp.push_back(*itL);
	//	}
	//}


	AdvancedDSTreeNode *node = _root;
	//node->update.push_back(x);
	bool isLeft;

	while (node->_leftChild != NULL)
	{
		isLeft = node->queryUpdateW(&node->update);
		if (isLeft)
		{
			node = node->_leftChild;
		}
		else
		{
			node = node->_rightChild;
		}
	}

	return gloverMatchingInLeafForAnX(node, x);
}

bool AdvancedDSTreeNode::queryUpdateW(vector<X>* updateList)
{
	vector<X>::iterator itL;
	vector<X> leftUpdateList, rightUpdateList;
	int leftSize = this->_leftChild->_values.size();
	bool isLeft = false;

	for (itL = updateList->begin(); itL != updateList->end(); itL++)
	{
		vector<X>::iterator itV = find(_matching.begin(), _matching.end(), *itL);
		if (itV != _matching.end())	// in matching
		{
			vector<X>::iterator tmp = find(lp.begin(), lp.end(), *itL);
			if (tmp != lp.end())	// in W
			{
				if (isXInLeftInQuery(*itV))
				{
					leftUpdateList.push_back(*itV);
					if (lp.size() > leftSize)	// W's size is greater than L.values
					{
						leftUpdateList.push_back(lp[leftSize]);
						rightUpdateList.push_back(lp[leftSize]);
					}	
					isLeft = true;
				}
				else
				{
					rightUpdateList.push_back(*itV);
				}
				lp.erase(itV);
			}
			else
			{
				rightUpdateList.push_back(*itV);
			}
			_matching.erase(itV);			
		}
		else // not in
		{
			vector<X>::iterator tmp = find(this->_leftChild->_matched.begin(), this->_leftChild->_matched.end(), *itL);					
			if (itL->_begin < this->_leftChild->_values[0] || tmp != this->_leftChild->_matched.end())	// not in W before
			{
				lp.push_back(*itL);
				if (isXInLeftInQuery(*itL))
				{
					leftUpdateList.push_back(*itL);
					if (lp.size() >= leftSize)	// W's size is greater than L.values
					{
						leftUpdateList.push_back(lp[leftSize]);
						rightUpdateList.push_back(lp[leftSize]);
					}
					isLeft = true;
				}
				else
				{
					rightUpdateList.push_back(*itL);
				}
			}
			else
			{
				rightUpdateList.push_back(*itL);
			}
			_matching.push_back(*itL);
		}
	}

	updateList->clear();
	for (int i = 0; i < leftUpdateList.size(); i++)
	{
		this->_leftChild->update.push_back(leftUpdateList[i]);
	}
	for (int i = 0; i < rightUpdateList.size(); i++)
	{
		this->_rightChild->update.push_back(rightUpdateList[i]);
	}

	return isLeft;
}

bool AdvancedDSTreeNode::isXInLeftInQuery(X x)
{
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
	sort(lp.begin(), lp.end(), cmpX2);
	
	if (cmpX2(lp[leftValues - 1], x))
	{
		isLeft = false;
	}
	else
	{
		isLeft = true;
	}

	return isLeft;
}


Y AdvancedDSTree::gloverMatchingInLeafForAnX(AdvancedDSTreeNode* leaf, X x)
{
	vector<X>::iterator itL;
	for (itL = leaf->update.begin(); itL != leaf->update.end(); itL++)
	{
		vector<X>::iterator itV = find(leaf->_matching.begin(), leaf->_matching.end(), *itL);
		if (itV != leaf->_matching.end())	// in matching
		{
			leaf->_matching.erase(itV);
		}
		else // not in
		{
			leaf->_matching.push_back(*itL);
		}
	}
	leaf->update.clear();

	sort(leaf->_matching.begin(), leaf->_matching.end(), cmpX2);

	Y y;

	for (int i = 0; i < leaf->_matching.size(); i++)
	{
		if (leaf->_matching[i] == x)
		{
			y = leaf->_values[i];
			break;
		}
	}
	return y;

}