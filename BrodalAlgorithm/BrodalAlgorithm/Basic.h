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
	bool operator<=(const Y& y)
	{
		return this->_y <= y._y;
	}
	bool operator>(const Y& y)
	{
		return this->_y > y._y;
	}
	bool operator>=(const Y& y)
	{
		return this->_y >= y._y;
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
	int _w;	// weight, assume it is positive
	bool operator==(const X& x)
	{
		return this->_id == x._id;
	}
	bool operator!=(const X& x)
	{
		return !(*this == x);
	}
};
ostream& operator<<(ostream& os, const X& rhs);

struct Msg
{
	bool _aEmpty; //true: _a is empty
	X _a;
	bool _bEmpty;
	X _b;
	int _c; //c==0, null; c==1, transferred; c==2, infeasible

	void operator=(const Msg& rhs)
	{
		this->_a = rhs._a;
		this->_aEmpty = rhs._aEmpty;
		this->_b = rhs._b;
		this->_bEmpty = rhs._bEmpty;
		this->_c = rhs._c;
	}
};







