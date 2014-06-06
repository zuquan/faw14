#include<iostream>
#include"define.h"

using namespace std;

class A
{
private:
	int value;
	void print()
	{
		cout << value << endl;
	}
public:
	A()
	{
		value = 10;
	}
	void test(A& a)
	{
		a.value = 100;
		a.print();
	}

};

int main()
{
	cout << "Hello" << endl;
	A a;
	A b;
	b.test(a);
	return 0; 
}