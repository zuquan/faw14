#pragma once

#include"AdvancedDSTree.h"

struct M
{
	X _x;
	Y _y;
};


class UnitTest
{

public:
	UnitTest(vector<X> , vector<Y>);
	vector<X> _allX;
	vector<Y> _allY;
	vector<X> _matchedX;
	vector<Y> _matchedY;
	vector<M> _M;
	void testTest();
	void testGloverMatching(vector<X> allX, vector<Y> allY, vector<X>& matchedX, vector<Y>& matchedY, vector<M>& );	
	void printUnitTest();
	bool isXMatched(X x);
	bool isYMatched(Y);
	Y mateOfX(X);
	X mateOfY(Y);

	void verifiyESTree(ESTreeNode* node);
	void testLocatIndexL(AdvancedDSTree *);
	void testEETree(AdvancedDSTree * tree);
	void testLbyK(AdvancedDSTreeNode *, X);

	void unitTestWeightXMatchedSet(AdvancedDSTree *);
	void UnitTest::printRootESTree(AdvancedDSTree *);
};
