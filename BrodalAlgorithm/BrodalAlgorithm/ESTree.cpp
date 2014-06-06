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
	buildTree(_root, NULL, 1, rangeOfY);
	/*int a = 1;
	verifiyESTree(_root, a);*/
}