#include "pch.h"
#include "BucketManager.h"


BucketManager::BucketManager()
{
}


BucketManager::~BucketManager()
{
	unordered_map<int, DocumentPointer*>::iterator iter;
	for (iter = hashtable.begin(); iter != hashtable.end(); iter++)
	{
		DocumentPointer* temp = iter->second;
		iter->second = NULL;
		while (temp)
		{
			DocumentPointer* deletetemp = temp;
			temp = temp->pointer;
			delete deletetemp;
		}
		delete iter->second;
	}
}

void BucketManager::InsertBand(int docid, string band, bool isString)
{
	DocumentPointer* dp = new DocumentPointer();
	dp->DocID = docid;
	dp->pointer = NULL;
	int h = 0;
	if (isString)
	{
		h = hash<string>{}(band);
	}
	else
	{
		h = hash<int>{}(stoi(band));
	}

	DocumentPointer* temp = hashtable[h];
	if (temp)
	{
		while (temp->pointer)
		{
			temp = temp->pointer;
		}
		temp->pointer = dp;
	}
	else
	{
		hashtable[h] = dp;
	}
}