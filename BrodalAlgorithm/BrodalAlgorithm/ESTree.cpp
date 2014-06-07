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
	currentRoot->_min = min;
	if (min == max)
	{
		currentRoot->_add = min;
		currentRoot->_min = 0;
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
		if (root->_leftChild == NULL)
		{
			cout << num++ << '\t' << "Leaf " << "\t\t" << root->_add << '\t' << root->_min << '\t' << root->_leafNum << endl;
		}
		else
		{
			cout << num++ << '\t' << "Internal " << '\t' << root->_add << '\t' << root->_min << '\t' << root->_leafNum << endl;
		}
		verifiyESTree((ESTreeNode*)root->_leftChild, num);
		verifiyESTree((ESTreeNode*)root->_rightChild, num);
	}

}

ESTree::ESTree(int rangeOfY)
{
	buildTree(_root, NULL, 0, rangeOfY);
	int a = 1;
	verifiyESTree(_root, a);
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
	//ESTreeNode* leafKMinusOne = locateLeafK(k - 1);//n_k+1=>b_{k-1}-1 so locate k-1
	//leafKMinusOne->_add--;
	//ESTreeNode* tmp = leafKMinusOne;
	//int tmpSumU2V = 0;
	//while (tmp != NULL)
	//{
	//	if (tmpSumU2V < tmp->_min)
	//		tmp->_min = tmpSumU2V;
	//	tmpSumU2V += tmp->_add;
	//	tmp = tmp->_parent;
	//}

	ESTreeNode* tmp = NULL;
	int tempSum = 0;
	ESTreeNode* leafK = locateLeafK(k-1);	// this is the implementation model, so k -> k-1
	tmp = leafK;//find the leaf corresponding to k(the k+1 the leaf)
	while (tmp != NULL)
	{
		tempSum += tmp->_add;
		tmp = tmp->_parent;
	}
	tmp = leafK;

	//int j = -1;
	ESTreeNode* nodeContainJ=NULL;
	ESTreeNode* leafJ;

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
					tempSum -= tmp->_add;
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
		if (nodeContainJ == NULL)
		{
			nodeContainJ = _root;
		}
	}

	if (nodeContainJ->_parent == NULL)	// in the root, i.e., j=m+1
	{
		tmp = nodeContainJ->_rightChild;
		while (tmp->_rightChild != NULL)
		{
			tmp = tmp->_rightChild;
		}
		leafJ = tmp;
	}
	else
	{
		tmp = nodeContainJ->_rightChild;
		while (tmp->_leftChild != NULL)
		{			
			tempSum += tmp->_add;
			if (tempSum + tmp->_leftChild->_add + tmp->_leftChild->_min < 0)
			{
				tmp = tmp->_leftChild;
			}
			else
			{
				tmp = tmp->_rightChild;
			}
		}
		leafJ = tmp;
	}

	stack<ESTreeNode*> qk = getPathElements(leafK);
	stack<ESTreeNode*> qj = getPathElements(leafJ);
	ESTreeNode* anc;
	while (qk.top() == qj.top())
	{
		anc = qk.top();
		qk.pop();
		qj.pop();
	}
	//lca is not in the stack
	ESTreeNode* lca = anc;

	if (lca == leafK)
	{
		leafK->_add--;
		updateMin(lca);
	}
	else
	{
		anc = qk.top();
		qk.pop();
		while (!qk.empty())
		{
			ESTreeNode* temp = qk.top();
			qk.pop();
			if (temp == anc->_leftChild)
			{
				anc->_rightChild->_add--;
				if (anc->_rightChild->_min + anc->_rightChild->_add < anc->_min)
				{
					anc->_min = anc->_rightChild->_min + anc->_rightChild->_add;
				}
			}
			anc = temp;
			
		}
		anc->_add--;
		updateMin(anc);

		anc = qj.top();
		qj.pop();
		while (!qj.empty())
		{
			ESTreeNode* temp = qj.top();
			qj.pop();
			if (temp == anc->_rightChild)
			{
				anc->_leftChild->_add--;
				if (anc->_leftChild->_min + anc->_leftChild->_add < anc->_min)
				{
					anc->_min = anc->_leftChild->_min + anc->_leftChild->_add;
				}
			}
			anc = temp;

		}
		anc->_add--;
		updateMin(anc);
	}

	int a = 1;
	verifiyESTree(this->_root, a);

	Msg msg;
	return msg;
}

void ESTree::updateMin(ESTreeNode* leaf)
{
	//ESTreeNode* leafKMinusOne = locateLeafK(k - 1);//n_k+1=>b_{k-1}-1 so locate k-1
	//leafKMinusOne->_add--;
	ESTreeNode* tmp = leaf;
	int tmpSumU2V = 0;
	while (tmp != NULL)
	{
		if (tmpSumU2V < tmp->_min)
			tmp->_min = tmpSumU2V;
		tmpSumU2V += tmp->_add;
		tmp = tmp->_parent;
	}
}

stack<ESTreeNode*> ESTree::getPathElements(ESTreeNode* node)
{
	stack<ESTreeNode*> v;
	ESTreeNode* tmp = node;
	while (true)
	{
		v.push(node);
		node = node->_parent;
		if (node == NULL)
		{
			break;
		}
		
	}
	return v;
}