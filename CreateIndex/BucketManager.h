#pragma once
#include "stdlib.h"
#include <string>
#include "Structures.h"

#include <vector>
#include <unordered_map>
#include <iostream>
using namespace std;

class BucketManager
{
public:
	BucketManager();
	~BucketManager();

	void InsertBand(int docid, string band, bool isString);

	unordered_map<int, DocumentPointer*> hashtable;

private:


};

