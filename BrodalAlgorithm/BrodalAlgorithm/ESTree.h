#pragma once

#include "Basic.h"

class ESTreeNode :public Node
{
private:
	/*//leaf
	int _b;
	//internal*/
	int _add;
	int _min;
	int _leafNum;
	friend class ESTree;

public:
	ESTreeNode(int leafNum);
};

class ESTree
{
private:
	ESTreeNode* _root;
	int findMinBj(int k);
	void decreaseAdd(int k, int j);

	//auxiluary fuction
	void buildTree(ESTreeNode* &currentRoot, ESTreeNode* parent, int min, int max);
	void verifiyESTree(ESTreeNode* root, int& num);

public:
	ESTree(int rangeOfY);
	Msg insertVariable(int k);
};

