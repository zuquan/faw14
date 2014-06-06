#pragma once

#include<iostream>
#include<set>
#include<list>

using namespace std;

struct Y
{
	int _id;
};


struct X
{
	int _id;
	Y _begin;
	Y _end;
};

struct Msg
{
	bool _aEmpty; //true: _a is empty
	X _a;
	bool _bEmpty;
	X _b;
	int _c; //c==0, null; c==1, transferred; c==2, infeasible
};

class Node
{
private:
	Node* _parent;
	Node* _leftChild;
	Node* _rightChild;
};

class ESTreeNode :public Node
{
private:
	/*//leaf
	int _b;
	//internal*/
	int _add;
	int _min;
	friend class ESTree;
};

class ESTree
{
private:
	ESTreeNode* _root;
	int findMinBj(int k);
	void decreaseAdd(int k, int j);
public:
	Msg insertVariable(int k);
};

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

	set<Y> _values;
	list<X> lp;
	list<X> update;

	ESTree _esTree;

public:
	void receive(Msg m);
	void initESTree();
};

class AdvancedDSTree
{
private:
	AdvancedDSNode* _root;
public:
	void insertX(X x);
	bool isXMatched(X x);
	bool isYMatched(Y y);
	Y queryXMate(X x);
	X queryYMate(Y y);
};



