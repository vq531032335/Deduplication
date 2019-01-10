#pragma once
#include <fstream>
#include <string>
#include "Structures.h"
using namespace std;


template<typename structname>
class MyFileWriter
//set certian type of structure into binary file
{
public:
	MyFileWriter()
	{}

	MyFileWriter(string filename)
	{
		out_file.open(filename, ios::out | ios::binary);
		struct_num = 0;
	}

	~MyFileWriter()
	{
		out_file.close();
	}

	void RefreshBuffer()
	// write the content of buffer into the file
	{
		out_file.write((char*)(&buffer[0]), sizeof(structname)*struct_num);
	}

	void SetStruct(structname str)
	//Each time set a structure into buffer 
	//if buffer meets its end, this function will automatically refresh the buffer
	//caution: dont forget to call RefreshBuffer() after all the setting work
	{
		buffer[struct_num] = str;
		struct_num++;

		if (struct_num == StructMaxNum)//buffer is full
		{
			RefreshBuffer();
			struct_num = 0;
		}
	}

private:
	const static int BufferSize = 20 * 1024 * 1024;// 20 MB. this number can be changed, which controls the size of writer buffer
	const static int StructMaxNum = BufferSize / sizeof(structname);
	structname buffer[StructMaxNum];

	int struct_num = 0;//number of structures in buffer
	ofstream out_file;
};

