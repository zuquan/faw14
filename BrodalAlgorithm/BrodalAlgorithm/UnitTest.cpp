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
					(selectedX[j]._end._y == x._end._y && selectedX[j]._id < x._id))
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

void generator()
{
	int range;
	cin >> range;
	int xnum;
	cin >> xnum;

	ofstream of("input.txt");

	of << range << endl;

	for (int i = 1; i <= range; i++)
	{
		of << i << " ";
	}
	//of << endl;


	vector<int> begin;
	vector<int> end;
	int diff = 0;

	for (int i = 1; i <= xnum; i++)
	{
		srand((unsigned)time(NULL) + diff);
		diff++;
		int b = rand() % range + 1;
		int rest = range - b;
		int e;
		if (rest == 0)
			e = b;
		else
		{
			srand((unsigned)time(NULL));
			diff++;
			int diff = rand() % rest;
			e = b + diff;
		}
		begin.push_back(b);
		end.push_back(e);
	}


	for (int i = 1; i <= xnum; i++)
	{
		of << endl;
		of << 1 << " ";
		of << i << " ";
		of << begin[i - 1] << " ";
		of << end[i - 1] << " ";
	}
	for (int i = 1; i <= xnum; i++)
	{
		of << endl;
		of << 6 << " " << i << " " << begin[i - 1] << " " << end[i - 1];
	}
	for (int i = 1; i <= range; i++)
	{
		of << endl;
		of << 8 << " " << i;
	}
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

	ofstream out("debug.txt");

	cout << "debug ESTREE format: (index, add, min, leafNum)" << endl;
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
			out << '(' << count++ << '|' << tmp->_add << '|' << tmp->_min << '|' << tmp->_leafNum << ')' << '\t';
		}
		out << endl;
	}	
}

