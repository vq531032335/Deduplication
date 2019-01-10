#pragma once
#include "MyFileWriter.h"
#include "MyFileReader.h"
#include "Structures.h"
#include <bitset>

using namespace std;

int VBEncode(MyFileWriter<char>* writer, unsigned int num);

int VBDecode(MyFileReader<char>* reader);


int ReadTermID(MyFileReader<char>* reader);

int WriteTermID(MyFileWriter<char>* writer, int termID);

