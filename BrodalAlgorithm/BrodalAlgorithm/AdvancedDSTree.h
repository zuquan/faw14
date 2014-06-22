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
	vector<X> _matched2;//matched'
	vector<X> lp;//W
	vector<X> update;
		//vector<X> _variables2;//variables'
		//vector<X> _transferred2;//transferred'
	

	ESTree* _pESTree;

	AdvancedDSTreeNode* _parent;
	AdvancedDSTreeNode* _leftChild;
	AdvancedDSTreeNode* _rightChild;
	void deleteCurrentESTree(ESTreeNode* currentNode);
	void deleteCurrentESTree();
	int sizeOfY(Y start, Y end);

	void queryUpdateNodeW();
	void queryUpdateLeafW();
	bool isXInLeftInQuery(X);

	void removeX(Msg m);	//for the case of msg fail-success and fail-fail expcetional.
	
	
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
	
	void splitDSNode(AdvancedDSTreeNode*, X);
	void updateAuxSet4Split(AdvancedDSTreeNode*);
	Y gloverMatchingInLeafForAnX(AdvancedDSTreeNode*, X);
	X gloverMatchingInLeafForAnY(AdvancedDSTreeNode*, Y);
	
public:
	AdvancedDSTree();
	bool insertX(X &x);
	bool isXMatched(X x);
	bool isYMatched(Y y);
	Y queryXMate(X x);
	X queryYMate(Y y);

	bool adjustXToProper(X& x);//success or not

	void verifiyDSTree(AdvancedDSTreeNode* root);
	void unitTestDS(string str);

};
