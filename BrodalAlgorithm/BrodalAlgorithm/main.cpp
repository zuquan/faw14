#include<iostream>
#include<fstream>

#include"define.h"

using namespace std;

int main()
{
	//cout << "hello" << endl;
	//open a file
	ifstream in("input.txt");
	if (!in)
	{
		cout << "input file open error" << endl;
		return 0;
	}
	//get the range of Y
	int rangeOfY;
	in >> rangeOfY;
	cout << rangeOfY << endl;
	AdvancedDSTree* pTree = new AdvancedDSTree(rangeOfY);
	
	in.close();
	return 0;
}