#include<algorithm>
#include"UnitTest.h"
#include<fstream>
#include<Windows.h>
#include<time.h>

bool cmpX_ID(X x1, X x2)
{
	return x1._id < x2._id;
}

bool cmpY_UT(Y y1, Y y2)
{
	return y1._y < y2._y;
}

// priority: increasing begin, increasing end, increasing id
bool cmpXBegInc_UT(X x1, X x2)
{
	if (x1._begin == x2._begin)
	{
		if (x1._end == x2._end)
		{
			return x1._id < x2._id;
		}
		else
		{
			return x1._end < x2._end;
		}
	}
	else
	{
		return x1._begin < x2._begin;
	}
}

void UnitTest::testTest()
{
	cout << "in UnitTest";
}

void UnitTest::printUnitTest()
{
	ofstream out("outputUT.txt");
	// out << "test";


	testGloverMatching(_allX, _allY, _matchedX, _matchedY, _M);
	sort(_allX.begin(), _allX.end(), cmpX_ID);
	for (int i = 0; i < _allX.size(); i++)
	{
		if (isXMatched(_allX[i]) == true)
		{
			out << "x:" << _allX[i]._id << " is matched with y:" << mateOfX(_allX[i]) << endl;
		}
		else
		{
			out << "x:" << _allX[i]._id << " is not matched." << endl;
		}
	}

	sort(_allY.begin(), _allY.end(), cmpY_UT);
	for (int i = 0; i < _allY.size(); i++)
	{
		if (isYMatched(_allY[i]) == true)
		{
			out << "y:" << _allY[i]._y << " is matched with x:" << mateOfY(_allY[i]) << endl;
		}
		else
		{
			out << "y:" << _allY[i]._y << " is not matched." << endl;
		}
	}

	out.close();
}

bool UnitTest::isXMatched(X x)
{
	vector<X>::iterator tmp = find(_matchedX.begin(), _matchedX.end(), x);
	if (tmp == _matchedX.end())	// not in matched set
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool UnitTest::isYMatched(Y y)
{
	vector<Y>::iterator tmp = find(_matchedY.begin(), _matchedY.end(), y);
	if (tmp == _matchedY.end())	// not in matched set
	{
		return false;
	}
	else
	{
		return true;
	}
}

Y UnitTest::mateOfX(X x)
{
	for (int i = 0; i < _M.size(); i++)
	{
		if (_M[i]._x == x)
		{
			return _M[i]._y;
		}
	}
}

X UnitTest::mateOfY(Y y)
{
	for (int i = 0; i < _M.size(); i++)
	{
		if (_M[i]._y == y)
		{
			return _M[i]._x;
		}
	}
}

void UnitTest::testGloverMatching(vector<X> inputX, vector<Y>inputY,
	vector<X>& resultX, vector<Y>& resultY, vector<M>& resultM)
{

	for (int i = 0; i < inputY.size(); i++)
	{
		vector<X> selectedX;
		for (int j = 0; j < inputX.size(); j++)
		{
			if (inputX[j]._begin._y <= inputY[i]._y && inputX[j]._end._y >= inputY[i]._y)
			{
				selectedX.push_back(inputX[j]);
			}
		}

		if (selectedX.size() == 0)
		{
			continue;
		}
		else
		{
			X x = selectedX[0];
			for (int j = 0; j < selectedX.size(); j++)
			{
				if (selectedX[j]._end._y < x._end._y ||
					(selectedX[j]._end._y == x._end._y && selectedX[j]._begin._y < x._begin._y)
					|| (selectedX[j]._end._y == x._end._y && selectedX[j]._begin._y == x._begin._y && selectedX[j]._id < x._id))
				{
					x = selectedX[j];
				}
			}

			resultX.push_back(x);
			resultY.push_back(inputY[i]);
			M m;
			m._x = x;
			m._y = inputY[i];
			resultM.push_back(m);

			vector<X>::iterator it = find(inputX.begin(), inputX.end(), x);
			inputX.erase(it);
		}
	}
}

UnitTest::UnitTest(vector<X> vX, vector<Y> vY)
{
	for (int i = 0; i < vX.size(); i++)
	{
		_allX.push_back(vX[i]);
	}
	for (int i = 0; i < vY.size(); i++)
	{
		_allY.push_back(vY[i]);
	}
}

void generator(char* fileName)
{

	int range = 5;	
	int xnum = 10;
	int maxw = 100;
	
	/*cout << "input range of Y" << endl;
	int range;
	cin >> range;
	int xnum;
	cout << "input the number of X" << endl;
	cin >> xnum;
	int maxw;
	cout << "input the max weight" << endl;
	cin >> maxw;*/
	
	ofstream of(fileName);

	of << range << endl;

	for (int i = 1; i <= range; i++)
	{
		of << i << " ";
	}
	//of << endl;


	vector<int> begin;
	vector<int> end;
	vector<int> weight;

	/*SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);*/
	/*int seed;
	cout << "input seed of rand" << endl;
	cin >> seed;
	srand(seed);*/

	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	srand(lpsystime.wMilliseconds);

	for (int i = 1; i <= xnum; i++)
	{
		int b = rand() % range + 1;
		int rest = range - b;
		int e;
		if (rest == 0)
			e = b;
		else
		{
			int diff = rand() % rest;
			e = b + diff;
		}
		begin.push_back(b);
		end.push_back(e);
		int w = rand() % maxw + 1;
		weight.push_back(w);
	}


	for (int i = 1; i <= xnum; i++)
	{
		of << endl;
		of << 1 << " ";
		of << i << " ";
		of << begin[i - 1] << " ";
		of << end[i - 1] << " ";
		of << weight[i - 1] << " ";
	}
	/*for (int i = 1; i <= xnum; i++)
	{
	of << endl;
	of << 6 << " " << i << " " << begin[i - 1] << " " << end[i - 1] << " "<<weight[i - 1];
	}
	for (int i = 1; i <= range; i++)
	{
	of << endl;
	of << 8 << " " << i;
	}*/
	of << endl;
	of << '$' << endl;


	of.close();



}


void UnitTest::verifiyESTree(ESTreeNode* node)
{
	ESTreeNode* root = node;
	while (root->_parent != NULL)
	{
		root = root->_parent;
	}

	ofstream out("debug.txt", ios_base::in | ios_base::app);

	// cout << "debug ESTREE format: (index, add, min, leafNum)" << endl;
	deque<ESTreeNode*> queue;
	queue.push_back(root);
	int count = 1;
	int level = 0;

	while (queue.empty() == false)
	{
		cout << "level " << level++ << ": ";
		int size = queue.size();
		for (int i = 0; i < size; i++)
		{
			ESTreeNode* tmp = queue.front();
			queue.pop_front();
			if (tmp->_leftChild != NULL)
			{
				queue.push_back(tmp->_leftChild);
				queue.push_back(tmp->_rightChild);
			}
			out << '(' << tmp->_add << '|' << tmp->_min << '|' << tmp->_leafNum << ')' << '\t';
			//out << '(' << count++ << '|' << tmp->_add << '|' << tmp->_min << '|' << tmp->_leafNum << ')' << '\t';
		}
		out << endl;
	}
}

void UnitTest::testLocatIndexL(AdvancedDSTree * tree)
{
	X x;
	Y y;
	y._y = 1;
	x._begin = y;
	y._y = 3;
	x._end = y;

	AdvancedDSTreeNode *node = tree->_root;

	vector<Y>* pESValues;
	if (node->_rightChild != NULL)
	{
		pESValues = &node->_rightChild->_values;
	}
	else
	{
		pESValues = &node->_values;
	}

	int kOfX = node->sizeOfY((*pESValues)[0], x._end);

	ESTreeNode* leafK = node->_pESTree->locateLeafK(3);	// this is the implementation model, so k -> k-1
	ESTreeNode* leafJ = node->_pESTree->locateLeafL(leafK);	// compute a_j=j

	//verifiyESTree(node->_pESTree->_root);
	int a = 0;
}


void UnitTest::testEETree(AdvancedDSTree * tree)
{
	int b = tree->_root->_pEETree->getLbyK(1);
	int a = 0;
}

void UnitTest::testLbyK(AdvancedDSTreeNode * node, X x)
{
	sort(node->_matched.begin(), node->_matched.end(), cmpXBegInc_UT);
	vector<X>::iterator it = find(node->_matched.begin(), node->_matched.end(), x);



}


void UnitTest::unitTestWeightXMatchedSet(AdvancedDSTree * tree)
{
	ofstream out("outputUT_FAW.txt");
	// out << "test";

	AdvancedDSTreeNode * node = tree->_root;
	sort(node->_matched.begin(), node->_matched.end(), cmpX_ID);

	for (int i = 0; i < node->_matched.size(); i++)
	{
		out << node->_matched[i]._id << endl;
	}

	out.close();
}