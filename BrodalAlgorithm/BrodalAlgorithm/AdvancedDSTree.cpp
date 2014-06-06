#include<algorithm>
#include"AdvancedDSTree.h"


extern vector<Y> allExistingY;

bool cmp(Y y1, Y y2)
{
	return y1._y < y2._y;
}


AdvancedDSNode::AdvancedDSNode(vector<Y> allY)
{
	//cout << "create estree " << rangeOfY << endl;
	_pESTree = new ESTree(allY.size());
	sort(allY.begin(), allY.end(), cmp);
	for (int i = 0; i < (int)allY.size(); i++)
	{
		_values.push_back(allY[i]);
	}
}

AdvancedDSTree::AdvancedDSTree()
{
	_root = new AdvancedDSNode(allExistingY);
}

void AdvancedDSTree::insertX(X x)
{

}

Y AdvancedDSNode::getIntervalStart()
{
	return _values[0];
}

AdvancedDSNode* AdvancedDSTree::locateLeafOfX(X x)
{
	AdvancedDSNode* tmp = _root;
	while (x._begin._y != tmp->getIntervalStart()._y)
	{
		if (tmp->_rightChild != NULL)
		{
			if (x._begin._y >= ((AdvancedDSNode*)tmp->_rightChild)->getIntervalStart()._y)
			{
				tmp = (AdvancedDSNode*)tmp->_rightChild;
			}
			else
			{
				tmp = (AdvancedDSNode*)tmp->_leftChild;
			}
		}
		else // assert there is no case that right child is NULL and left child is not NULL.
		{
			splitDSNode(tmp, x);
			return (AdvancedDSNode*)tmp->_rightChild;
		}
	}
	return tmp;
}

void AdvancedDSTree::splitDSNode(AdvancedDSNode* node, X x)
{
	vector<Y> leftVec, rightVec;
	int i = 0;
	while (node->_values[i]._y < x._begin._y)
	{
		leftVec.push_back(node->_values[i]);
		i++;
	}
	while (i < node->_values.size())
	{
		rightVec.push_back(node->_values[i]);
		i++;
	}

	AdvancedDSNode* leftChild = new AdvancedDSNode(leftVec);
	AdvancedDSNode* rightChild = new AdvancedDSNode(rightVec);

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
	return true;
}