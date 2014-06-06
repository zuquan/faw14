#pragma once

#include<iostream>
#include<set>
#include<list>
#include<vector>

using namespace std;

struct Y
{
	double _y;
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
	friend class ESTree;
	friend class AdvancedDSTree;
};






