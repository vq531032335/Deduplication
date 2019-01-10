#pragma once

#include "Structures.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class MyBufferReader
//alternative version of MyFileReader, used to handle strings
{
public:
	MyBufferReader();
	MyBufferReader(string filename);
	MyBufferReader(string filename, int iTermSart, int iTermEnd);
	~MyBufferReader();
	bool RefreshBuffer();
	string GetOneWord();
private:
	int iReadEnd;
	int iReadPos;
	const static int bufferSize = 5 * 1024 * 1024;
	char buffer[bufferSize + 1];//5MB buffer
	ifstream infile;

	char* cBufferPoint;//current reading pointer of buffer
	bool isBufferSpaceStart;//whether there is a space at the start of buffer 
	bool isBufferSpaceEnd;//whether there is a space at the end of buffer 
};

