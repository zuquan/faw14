#include<iostream>
#include<fstream>

#include"AdvancedDSTree.h"
#include"UnitTest.h"

using namespace std;

vector<Y> allExistingY;	// TBD: use BST to store Y nodes
vector<X> allExistingX;
UnitTest * ut;

void generator(char *);
ofstream olog("replaceLogFAW.txt");


int main()
{
	//open a file
	generator("input.txt");
	ifstream in("input.txt");
	ofstream out("outputMain.txt");
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
				  in >> x._id >> x._begin._y >> x._end._y >> x._w;
				  if (pTree->adjustXToProper(x) == false)
				  {
					  cout << x._id << " insert fail" << endl;					  
					  break;
				  }		
				  else
				  {
					  allExistingX.push_back(x);	// to be checked
				  }				  

				  if (x._id == 9)
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
					/*if (x._id == 1)
					{
						int a = 0;
					}
					if (pTree->adjustXToProper(x) == false)
					{
						cout << x._id << "illegal x " << endl;
						out << "x:" << x._id << " is not matched." << endl;
						break;
					}
					else
					{
						if (pTree->isXMatched(x) == true)
						{
							out << "x:" << x._id << " is matched with y:" << pTree->queryXMate(x) << endl;
						}
						else
						{
							out << "x:" << x._id << " is not matched." << endl;
						}
					}*/
					
		}break;
		case '7':
		{

		}break;
		case '8':
		{
					Y y;
					in >> y._y;
					/*if (y._y == 1)
					{
						int a = 0;
					}
					if (pTree->isYMatched(y) == true)
					{
						out << "y:" << y._y << " is matched with x:" << pTree->queryYMate(y) << endl;
					}
					else
					{
						out << "y:" << y._y << " is not matched." << endl;
					}*/
		}break;
		}

	}
	
	//pTree->unitTestDS("DSTREE");	// unit test
	cout << "end" << endl;
	olog.close();

	ut = new UnitTest(allExistingX, allExistingY);
	ut->unitTestWeightXMatchedSet(pTree);



	//ut->printUnitTest();

	//ut->testEETree(pTree);

	//for (int i = 0; i < allExistingX.size(); i++)
	//{
	//	X x = allExistingX[i];
	//	if (pTree->isXMatched(x) == true)
	//	{
	//		out << "x:" << x._id << " is matched with y:" << pTree->queryXMate(x) << endl;
	//	}
	//	else
	//	{
	//		out << "x:" << x._id << " is not matched." << endl;
	//	}
	//}

	//for (int i = 0; i < allExistingY.size(); i++)
	//{
	//	Y y = allExistingY[i];
	//	if (pTree->isYMatched(y) == true)
	//	{
	//		out << "y:" << y._y << " is matched with x:" << pTree->queryYMate(y) << endl;
	//	}
	//	else
	//	{
	//		out << "y:" << y._y << " is not matched." << endl;
	//	}
	//}

	//// unit test
	//ut->testLocatIndexL(pTree);
	
	

	in.close();
	return 0;
}