#pragma once

#include"ESTree.h"


class AdvancedDSNode : public Node
{
private:
	set<X> _variables;
	set<X> _variables2;//variables'
	set<X> _matched;
	set<X> _matching;
	set<X> _transferred;
	set<X> _transferred2;//transferred'
	set<X> _infeasible;
	set<X> _matched2;//matched'

	vector<Y> _values;
	list<X> lp;//W
	list<X> update;

	ESTree* _pESTree;
	
public:
	AdvancedDSNode(vector<Y>);
	void receive(Msg m);
	void initESTree();
	Y getIntervalStart();
	friend class AdvancedDSTree;
};

class AdvancedDSTree
{
private:
	AdvancedDSNode* _root;
	AdvancedDSNode* locateLeafOfX(X x);	
	bool adjustXToProper(X& x);//success or not
	void splitDSNode(AdvancedDSNode*, X);
	
public:
	AdvancedDSTree();
	void insertX(X x);
	bool isXMatched(X x);
	bool isYMatched(Y y);
	Y queryXMate(X x);
	X queryYMate(Y y);

};
