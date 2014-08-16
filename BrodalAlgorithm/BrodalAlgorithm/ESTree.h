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
	friend class UnitTest;

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
	ESTreeNode* locateLeafJ(ESTreeNode*);
	void updateBjFromK2J(ESTreeNode* leafK, ESTreeNode* leafJ, int diff);

	ESTreeNode* locateLeafL(ESTreeNode*);	// the a_j=j before k
	

	//auxiluary fuction
	void buildTree(ESTreeNode* &currentRoot, ESTreeNode* parent, int min, int max);
	void updateMin(ESTreeNode*);
	stack<ESTreeNode*> getPathElements(ESTreeNode*);

	int getIndex(ESTreeNode*); //the index of the first leaf is 1

	friend class UnitTest;

public:
	ESTree(int rangeOfY);
	int insertVariable(int k);	//if the return value == m+1,then success, else failure
	friend class AdvancedDSTreeNode;

	void verifiyESTree(ESTreeNode* root, int& num);
	void unitTest(string str);

	int allLeafNum();

	void deleteVariable(int);
	void appendVariable(int);

	int getLbyK(int);
	int get2ndLbyK(int);
};

