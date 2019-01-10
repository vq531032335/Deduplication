#pragma once
#include <fstream>
#include <string>
#include "Structures.h"
using namespace std;


template<typename structname>
class MyFileReader
//get certian type of structure from binary file
{
public:
	MyFileReader()
	{}

	MyFileReader(string filename)
	{
		in_file.open(filename, ios::in | ios::binary);
		iFilePos = -1;
		iFileLen = -1;
		struct_num = 0;
	}

	MyFileReader(string filename, int iStart, int iLen)//if iLen means no limit
	//only access to certain part of file
	{
		in_file.open(filename, ios::in | ios::binary);

		in_file.seekg(iStart, ios::beg);
		iFilePos = 0;
		iFileLen = iLen;
		struct_num = 0;
	}

	~MyFileReader()
	{
		in_file.close();
	}

	void RefreshBuffer()
	// read the content of file into the buffer
	{
		in_file.read((char*)(&buffer[0]), sizeof(structname)*StructMaxNum);

		now_struct_max_num = in_file.gcount();
	}

	structname* GetStruct()
	//return a pointer (succesfully), or NULL (failed)
	//caution: dont forget delete the result after using it
	{
		structname* result = NULL;
		if (iFilePos == -1 || iFileLen == -1 || iFilePos < iFileLen)
		{
			if (struct_num == now_struct_max_num)//buffer is empty
			{
				RefreshBuffer();
				if (now_struct_max_num == 0)return NULL;//file is empty
				struct_num = 0;
			}
			result = new structname(buffer[struct_num]);
			if(iFilePos!=-1)iFilePos += sizeof(structname);
			struct_num++;
		}
		return result;
	}


private:
	const static int BufferSize = 5 * 1024 * 1024;// 5 MB. this number can be changed, which controls the size of reader buffer
	const static int StructMaxNum = BufferSize / sizeof(structname);
	structname buffer[StructMaxNum];

	int now_struct_max_num = 0;
	int struct_num = 0;//number of structures been output from buffer
	ifstream in_file;

	int iFilePos;
	int iFileLen;
};

