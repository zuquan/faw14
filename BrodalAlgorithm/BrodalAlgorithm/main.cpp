#include<iostream>
#include<fstream>

#include"AdvancedDSTree.h"

using namespace std;

vector<Y> allExistingY;	// TBD: use BST to store Y nodes
vector<X> allExistingX;

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

	for (int i = 0; i < rangeOfY; i++)
	{
		Y temp;
		in >> temp._y;
		allExistingY.push_back(temp);
	}

	AdvancedDSTree* pTree = new AdvancedDSTree();

	char command;
	while (!in.eof())
	{
		in >> command;
		if (command == '#')
		{
			char s[20];
			in.get(s, 20, '\n');
			continue;
		}
		if (command == '$')
		{
			break;
		}
			
		switch (command)
		{
			//insert an X
		case '1':
		{
				  X x;
				  in >> x._id >> x._begin._y >> x._end._y;
				  if (x._id == 16)
				  {
					  int a = 0;
				  }
				  pTree->insertX(x);
		}break;


		case '2':
		{

		}break;
		case '3':
		{

		}break;
		case '4':
		{

		}break;
		case '5':
		{

		}break;
		case '6':
		{
					X x;
					in >> x._id >> x._begin._y >> x._end._y;
					if (x._id == 1)
					{
						int a = 0;
					}
					if (pTree->isXMatched(x) == true)
					{
						cout << "x:" << x._id << " is matched with y:" << pTree->queryXMate(x) << endl;
					}
					else
					{
						cout << "x:" << x._id << " is not matched." << endl;
					}
					
		}break;
		case '7':
		{

		}break;
		case '8':
		{
		}break;
		}

	}
	
	//pTree->unitTestDS("DSTREE");	// unit test
	cout << "end" << endl;


	in.close();
	return 0;
}