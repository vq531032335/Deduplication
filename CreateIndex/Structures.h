#pragma once
#include <string>
using namespace std;

struct MyDocument_sim
{
	string URL;
	bool isBad;
	uint64_t value;
};

struct MyDocument_min
{
	string URL;
	bool isBad;
	int signature[20];
};

struct DocumentPointer
{
	int DocID;
	DocumentPointer* pointer;
};
