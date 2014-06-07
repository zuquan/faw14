#include<algorithm>
#include"AdvancedDSTree.h"


extern vector<Y> allExistingY;

bool cmp(Y y1, Y y2)
{
	return y1._y < y2._y;
}

ostream& operator<<(ostream& os, const Y& rhs)
{
	os << rhs._y;
	return os;
}

ostream& operator<<(ostream& os, const X& rhs)
{
	os <<"X: "<< rhs._id<<" "<<rhs._begin<<" "<<rhs._end;
	return os;
}

AdvancedDSTreeNode::AdvancedDSTreeNode(vector<Y> allY)
{
	//cout << "create estree " << rangeOfY << endl;
	_pESTree = new ESTree(allY.size());
	sort(allY.begin(), allY.end(), cmp);
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
			node = (AdvancedDSTreeNode*)node->_rightChild;
		}
	}
	while (node->_leftChild != NULL)
	{
		node->_variables.push_back(x);
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

	//_pESTree = new ESTree(rightVecY.size())
	node->deleteCurrentESTree();
	node->_pESTree = new ESTree(rightVecY.size());

	leftChild->_variables = node->_variables;	// copy the variables from the parent
	
	node->_leftChild = leftChild;
	node->_rightChild = rightChild;
	leftChild->_parent = node;
	rightChild->_parent = node;
}

bool AdvancedDSTree::adjustXToProper(X& x)
{
	
	int i = 0;
	double tempBegin;
	sort(allExistingY.begin(), allExistingY.end(), cmp);
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
	cout << x<< endl;
	return true;
}

bool AdvancedDSTree::insertX(X &x)
{
	if (adjustXToProper(x) == false)
	{
		cout << x._id << " insert fail" << endl;
		return false;
	}
	AdvancedDSTreeNode* leaf = locateLeafOfX(x);
	//leaf->_variables.push_back(x);
	
	// to do the op of insert variable
	//transfer X to (1,k)
	leaf->_pESTree->insertVariable(3);

	return true;
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