#pragma once

#include"ESTree.h"


class AdvancedDSTreeNode /*: public Node*/
{
private:
	vector<X> _variables;
	vector<X> _matched;
	vector<X> _transferred;
	vector<X> _infeasible;
	vector<Y> _values;
		vector<X> _matching;
		list<X> lp;//W
		list<X> update;
		//vector<X> _variables2;//variables'
		//vector<X> _transferred2;//transferred'
		//vector<X> _matched2;//matched'

	ESTree* _pESTree;

	AdvancedDSTreeNode* _parent;
	AdvancedDSTreeNode* _leftChild;
	AdvancedDSTreeNode* _rightChild;
	void deleteCurrentESTree(ESTreeNode* currentNode);
	void deleteCurrentESTree();
	int sizeOfY(Y start, Y end);
	
public:
	AdvancedDSTreeNode(vector<Y>);
	void receive(Msg m);
	void initESTree();
	Y getIntervalStart();
	friend class AdvancedDSTree;
	Msg insertX(X x);
};

class AdvancedDSTree
{
private:
	AdvancedDSTreeNode* _root;
	AdvancedDSTreeNode* locateLeafOfX(X x);	
	bool adjustXToProper(X& x);//success or not
	void splitDSNode(AdvancedDSTreeNode*, X);
	void updateAuxSet4Split(AdvancedDSTreeNode*);
	
public:
	AdvancedDSTree();
	bool insertX(X &x);
	bool isXMatched(X x);
	bool isYMatched(Y y);
	Y queryXMate(X x);
	X queryYMate(Y y);

};
