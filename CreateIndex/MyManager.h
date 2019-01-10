#pragma once

#include "MyBufferReader.h"
#include "MyBufferWriter.h"
#include "MyFileReader.h"
#include "MyFileWriter.h"
#include "VarByte.h"
#include "Structures.h"
#include "stdlib.h"
#include "time.h"
#include "BucketManager.h"
#include <io.h>
#include <iostream>
#include <string>

#include <vector>
#include <unordered_map>

using namespace std;

class MyManager
//main part of this program
{
public:
	MyManager();
	~MyManager();
	void changeMode(int n);
	void InitializeMinHash();
	int MinHash(int x, int No);

	void GetFiles(string path);//put all the files under path directory into a list called vsfiles
	string LoadFile();//get the name of next unread file. if no file left, returns ""
	   
	bool Deduplicate();//read all the pages, put postings into a heap until it is full, then output this heap and loop
	bool isEnglish(string& word);
	void MeetURL(string URL);
	void MeetTerm(string term);

	void Output_resultSimHash(int good, int total);
	void Clear_resultSimHash();

	void StartHash();
	int  CompareDoc(int x, int y);

	int StrToWeight(string word);
	long FNV1A_64_HASH(string k);

	void ShowBucket(int bucketNO, MyBufferWriter * bw);
private:
	static const int BandNumber = 4;
	static const int BandSize = 5;
	BucketManager bm[BandNumber];

	int mode = 1;// 1:minhash  2:simhash
	static const int MinHashSize = 20;//4*5
	static const int MinHashThreshold = 10;
	static const int SimHashSize = 64;
	static const int SimHashThreshold = 56;

	int metascore[SimHashSize];

	vector<string> vsfiles;//reading files list
	int iLoadFilenum;//reading file No

	vector<MyDocument_sim> documentlist_sim;
	vector<MyDocument_min> documentlist_min;
	unordered_map<string, int> termIDMap;
	pair<int, int> minHashArgs[MinHashSize];//10 be changed same as the number of signature in MyDocument_min
	int resultSimHash[SimHashSize];

};

