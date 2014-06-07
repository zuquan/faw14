#pragma once

#include"ESTree.h"


class AdvancedDSTreeNode /*: public Node*/
{
private:
	vector<X> _variables;
	vector<X> _variables2;//variables'
	vector<X> _matched;
	vector<X> _matching;
	vector<X> _transferred;
	vector<X> _transferred2;//transferred'
	vector<X> _infeasible;
	vector<X> _matched2;//matched'

	vector<Y> _values;
	list<X> lp;//W
	list<X> update;

	ESTree* _pESTree;

	AdvancedDSTreeNode* _parent;
	AdvancedDSTreeNode* _leftChild;
	AdvancedDSTreeNode* _rightChild;
	void deleteCurrentESTree(ESTreeNode* currentNode);
	void deleteCurrentESTree();
	
public:
	AdvancedDSTreeNode(vector<Y>);
	void receive(Msg m);
	void initESTree();
	Y getIntervalStart();
	friend class AdvancedDSTree;
};

class AdvancedDSTree
{
private:
	AdvancedDSTreeNode* _root;
	AdvancedDSTreeNode* locateLeafOfX(X x);	
	bool adjustXToProper(X& x);//success or not
	void splitDSNode(AdvancedDSTreeNode*, X);
	
public:
	AdvancedDSTree();
	bool insertX(X &x);
	bool isXMatched(X x);
	bool isYMatched(Y y);
	Y queryXMate(X x);
	X queryYMate(Y y);

};
