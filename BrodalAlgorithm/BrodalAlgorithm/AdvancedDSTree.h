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
	vector<X> _transFromLMatch;	// intersection of L.transferred and matched2
		//vector<X> _variables2;//variables'
		//vector<X> _transferred2;//transferred'
	

	ESTree* _pESTree;
	ESTree* _pEETree;

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
	void appendX(Msg m);

	void removeXinWeightProcess(X x);
	void appendXinWeightProcess(X x);

	void replaceMinX();		// find the min replaceable x and repalce it.
	bool findExtensionX();	// whether there is a extension x from transferred L
	
	vector<Y> getESValues();

	AdvancedDSTreeNode* pullBackATransferredXInWeightProcess(AdvancedDSTreeNode* infeasibleNode, X minWeightX);
	
public:
	AdvancedDSTreeNode(vector<Y>);
	void receive(Msg m);
	void initESTree();
	Y getIntervalStart();
	friend class AdvancedDSTree;
	friend class UnitTest;
	Msg insertX(X x);
};

class AdvancedDSTree
{
//public:
private:
	AdvancedDSTreeNode* _root;
	AdvancedDSTreeNode* locateLeafOfX(X x);	
	
	void splitDSNode(AdvancedDSTreeNode*, X);
	void updateAuxSet4Split(AdvancedDSTreeNode*);
	Y gloverMatchingInLeafForAnX(AdvancedDSTreeNode*, X);
	X gloverMatchingInLeafForAnY(AdvancedDSTreeNode*, Y);

	void replaceableSetOfP(AdvancedDSTreeNode*, X, X, vector<X> &);
	void repalceableSetOfLeftChild(AdvancedDSTreeNode*, X, vector<X> &);
	X determineMinWeightX(AdvancedDSTreeNode* infeasibleNode, X, X, AdvancedDSTreeNode*&);
	bool continueCalToLeft(AdvancedDSTreeNode* curNode, vector<X>& replaceableX);

	X replaceMinWeightX(AdvancedDSTreeNode*, Msg);
	
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

	friend class UnitTest;



};
