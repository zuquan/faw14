#pragma once

#include "Basic.h"

class ESTreeNode 
{
private:
	/*//leaf
	int _b;
	//internal*/

	ESTreeNode* _parent;
	ESTreeNode* _leftChild;
	ESTreeNode* _rightChild;

	int _add;
	int _min;
	int _leafNum;
	friend class ESTree;
	friend class AdvancedDSTreeNode;

public:
	ESTreeNode(int leafNum);
};

class ESTree
{
private:
	ESTreeNode* _root;
	int findMinBj(int k);
	void decreaseAdd(int k, int j);
	ESTreeNode* locateLeafK(int k);

	//auxiluary fuction
	void buildTree(ESTreeNode* &currentRoot, ESTreeNode* parent, int min, int max);
	void verifiyESTree(ESTreeNode* root, int& num);

public:
	ESTree(int rangeOfY);
	Msg insertVariable(int k);
	friend class AdvancedDSTreeNode;
};

