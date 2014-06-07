#include"ESTree.h"

ESTreeNode::ESTreeNode(int leafNum)
{
	_leafNum = leafNum;
	_add = 0;
	_min = 0;
}

void ESTree::buildTree(ESTreeNode* &currentRoot, ESTreeNode* parent, int min, int max)
{
	currentRoot = new ESTreeNode(max - min + 1);
	currentRoot->_parent = parent;
	if (min == max)
	{
		currentRoot->_leftChild = NULL;
		currentRoot->_rightChild = NULL;
	}
	else
	{
		int total = max - min + 1;
		int half = (total + 1) / 2;
		int mid = min + half - 1;
		buildTree((ESTreeNode* &)(currentRoot->_leftChild), currentRoot, min, mid);
		buildTree((ESTreeNode* &)(currentRoot->_rightChild), currentRoot, mid + 1, max);
	}
}

void ESTree::verifiyESTree(ESTreeNode* root, int& num)
{
	if (root != NULL)
	{
		cout << num++ << " : " << root->_leafNum << endl;
		verifiyESTree((ESTreeNode*)root->_leftChild, num);
		verifiyESTree((ESTreeNode*)root->_rightChild, num);
	}
}

ESTree::ESTree(int rangeOfY)
{
	buildTree(_root, NULL, 0, rangeOfY);
	/*int a = 1;
	verifiyESTree(_root, a);*/
}

ESTreeNode* ESTree::locateLeafK(int k)
{
	k++;//locate the leaf corresponding to k means find the (k+1)th leaf in ESTree
	ESTreeNode* temp = _root;
	while (true)
	{
		if (temp->_leftChild == NULL)
		{
			break;
		}
		//cout << temp->_leafNum << endl;
		if (k > temp->_leftChild->_leafNum)
		{
			k -= temp->_leftChild->_leafNum;
			temp = temp->_rightChild;
			//cout << "to right" << endl;
		}
		else
		{
			temp = temp->_leftChild;
			//cout << "to left" << endl;
		}
		
	}
	return temp;
}


Msg ESTree::insertVariable(int k)
{
	ESTreeNode* leafKMinusOne = locateLeafK(k - 1);//n_k+1=>b_{k-1}-1 so locate k-1
	leafKMinusOne->_add--;
	ESTreeNode* tmp = leafKMinusOne;
	int tmpSumU2V = 0;
	while (tmp != NULL)
	{
		if (tmpSumU2V < tmp->_min)
			tmp->_min = tmpSumU2V;
		tmpSumU2V += tmp->_add;
		tmp = tmp->_parent;
	}

	int tempSum = 0;
	ESTreeNode* leafK = locateLeafK(k);
	tmp = leafK;//find the leaf corresponding to k(the k+1 the leaf)
	while (tmp != NULL)
	{
		tempSum += tmp->_add;
		tmp = tmp->_parent;
	}
	tmp = leafK;

	//int j = -1;
	ESTreeNode* nodeContainJ;

	if (tempSum < 0)
	{
		nodeContainJ = tmp;
	}
	else
	{
		//tempSum -= tmp->_add;		
		while (tmp->_parent != NULL)
		{			
			if (tmp == tmp->_parent->_rightChild)
			{
				tempSum -= tmp->_add;
				tmp = tmp->_parent;
			}
			else
			{				
				ESTreeNode* sibling = tmp->_parent->_rightChild;
				int i = tempSum - tmp->_add + sibling->_add + sibling->_min;
				if (i < 0)
				{
					nodeContainJ = tmp->_parent;
					break;
				}
				else
				{
					tempSum -= tmp->_add;
					tmp = tmp->_parent;
				}
			}
		}

		

		//while (tmp != NULL && tempSum >= 0) // && tmp == tmp->_parent->_rightChild
		//{
		//	tempSum -= tmp->_add;
		//	if (tmp == tmp->_parent->_rightChild)
		//}
	}

	


	while (true)
	{
		if (tempSum < 0)
		{
			j = k;
			break;
		}
		else
		{
			
		}
	}



	Msg msg;
	return msg;
}