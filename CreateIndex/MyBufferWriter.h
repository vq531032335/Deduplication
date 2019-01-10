#pragma once

#include"Structures.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


class MyBufferWriter
//alternative version of MyFileWriter, used to handle strings
{
public:
	MyBufferWriter();
	MyBufferWriter(string filename);
	~MyBufferWriter();
	void RefreshBuffer();
	void SetOneWord(string word);
private:
	int iBufferSize = 20 * 1024 * 1024;
	char buffer[20 * 1024 * 1024];//20MB buffer

	ofstream out_file;
	int buffer_num;
	int buffer_point;//current writing pointer of buffer
};

