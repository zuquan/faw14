#pragma once

#include<iostream>
#include<set>
#include<list>
#include<vector>
#include<stack>

using namespace std;

struct Y
{
	double _y;
	bool operator<(const Y& y)
	{
		return this->_y < y._y;
	}
	bool operator>(const Y& y)
	{
		return this->_y > y._y;
	}
	bool operator==(const Y& y)
	{
		return this->_y == y._y;
	}
	bool operator!=(const Y& y)
	{
		return this->_y != y._y;
	}
	
};

ostream& operator<<(ostream& os, const Y& rhs);



struct X
{
	int _id;
	Y _begin;
	Y _end;
};
ostream& operator<<(ostream& os, const X& rhs);

struct Msg
{
	bool _aEmpty; //true: _a is empty
	X _a;
	bool _bEmpty;
	X _b;
	int _c; //c==0, null; c==1, transferred; c==2, infeasible
};

//class Node
//{
//private:
//	Node* _parent;
//	Node* _leftChild;
//	Node* _rightChild;
//	friend class ESTree;
//	friend class AdvancedDSTree;
//};






