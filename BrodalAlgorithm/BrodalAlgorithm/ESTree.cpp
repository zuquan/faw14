#include"ESTree.h"

ESTreeNode::ESTreeNode(int leafNum)
{
	_leafNum = leafNum;
	_add = 0;
	_min = 0;
}

// build ESTree, range from min to max
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

void ESTree::unitTest(string str)
{
	/*if (str == "ESTREE")
	{		
		cout << "ID" << '\t' << "Node/Leaf " << '\t' << "_add" << '\t' << "_min" << '\t' << "_leafNum" << endl;
		int a = 1;
		verifiyESTree(_root, a);
	}	*/
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

// initial ESTree from a size
ESTree::ESTree(int rangeOfY)
{
	buildTree(_root, NULL, 0, rangeOfY);
	//cout << "build ESTree of size " << rangeOfY << endl;

	unitTest("ESTREE");	// for test	
}

// locate the k^th leaf in the ESTree
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

// locate the leaf of a_j=j
ESTreeNode* ESTree::locateLeafJ(ESTreeNode* leafK)
{
	ESTreeNode* tmp = leafK;	//find the leaf corresponding to k(the k+1 the leaf)
	int tempSum = 0;	
	while (tmp != NULL)	// compute sum(k)
	{
		tempSum += tmp->_add;
		tmp = tmp->_parent;
	}
	tmp = leafK;

	ESTreeNode* nodeContainJ = NULL;
	ESTreeNode* leafJ = NULL;

	// find the internal node containing j
	if (tempSum < 0)	// leaf j=k
	{
		return leafK;
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

	// find the leaf j
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

	return leafJ;
}

// update b_j from the leaf k to the leaf j
void ESTree::updateBjFromK2J(ESTreeNode* leafK, ESTreeNode* leafJ)
{
	if (leafK == leafJ)
	{
		leafK->_add--;
		updateMin(leafK);
		return;
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
}


int ESTree::insertVariable(int k)
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

	if (k > _root->_leafNum - 1)
	{
		k = _root->_leafNum - 1;
	}
	
	ESTreeNode* leafK = locateLeafK(k - 1);	// this is the implementation model, so k -> k-1			
	ESTreeNode* leafJ = locateLeafJ(leafK);	// compute a_j=j
	updateBjFromK2J(leafK, leafJ);			// update the value of b_j or _add for the leaf or nodes between k and j, implictly.
	

	/*int a = getIndex(leafJ);
	int b = getIndex(leafK);*/
	unitTest("ESTREE");	// for test	

	int j = getIndex(leafJ);
	return j;
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


int ESTree::getIndex(ESTreeNode* leaf)
{
	ESTreeNode* tmp = leaf;
	int index = 1;
	
	while (tmp->_parent != NULL)
	{
		if (tmp == tmp->_parent->_rightChild)
		{
			index += tmp->_parent->_leftChild->_leafNum;
		}
		tmp = tmp->_parent;
	}

	return index;
}

int ESTree::allLeafNum()
{
	return _root->_leafNum;
}