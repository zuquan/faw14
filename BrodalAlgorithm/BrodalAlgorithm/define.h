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
	friend class ESTree;
	friend class AdvancedDSTree;
};

class ESTreeNode :public Node
{
private:
	/*//leaf
	int _b;
	//internal*/
	int _add;
	int _min;
	int _leafNum;
	friend class ESTree;

public:
	ESTreeNode(int leafNum)
	{
		_leafNum = leafNum;
		_add = 0;
		_min = 0;
	}
};

class ESTree
{
private:
	ESTreeNode* _root;
	int findMinBj(int k);
	void decreaseAdd(int k, int j);

	//auxiluary fuction
	void buildTree(ESTreeNode* &currentRoot, ESTreeNode* parent, int min, int max)
	{
		currentRoot = new ESTreeNode(max - min + 1);
		currentRoot->_parent = parent;
		if (min == max)
		{
			currentRoot->_leftChild = NULL;
			currentRoot->_rightChild = NULL;
		}
		else
		{
			int total = max - min + 1;
			int half = (total + 1) / 2;
			int mid = min + half - 1;
			buildTree((ESTreeNode* &)(currentRoot->_leftChild), currentRoot, min, mid);
			buildTree((ESTreeNode* &)(currentRoot->_rightChild), currentRoot, mid + 1, max);
		}
	}
	void verifiyESTree(ESTreeNode* root, int& num);

public:
	ESTree(int rangeOfY)
	{
		buildTree(_root, NULL, 1, rangeOfY);
		/*int a = 1;
		verifiyESTree(_root, a);*/
	}
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

	ESTree* _pESTree;

public:
	AdvancedDSNode(int rangeOfY)
	{
		//cout << "create estree " << rangeOfY << endl;
		_pESTree = new ESTree(rangeOfY);
	}
	void receive(Msg m);
	void initESTree();


};

class AdvancedDSTree
{
private:
	AdvancedDSNode* _root;
public:
	AdvancedDSTree(int rangeOfY)
	{
		_root = new AdvancedDSNode(rangeOfY);
	}
	void insertX(X x);
	bool isXMatched(X x);
	bool isYMatched(Y y);
	Y queryXMate(X x);
	X queryYMate(Y y);

};


void ESTree::verifiyESTree(ESTreeNode* root, int& num)
{
	if (root != NULL)
	{
		cout << num++ << " : " << root->_leafNum << endl;
		verifiyESTree((ESTreeNode*)root->_leftChild, num);
		verifiyESTree((ESTreeNode*)root->_rightChild, num);
	}
}
