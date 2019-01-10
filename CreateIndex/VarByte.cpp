#include "pch.h"
#include "VarByte.h"


int VBEncode(MyFileWriter<char>* writer, unsigned int num)
//compress a integer into VAR BYTE into the file, returns the length
{

	uint8_t b;
	int result = 0;
	while (num >= 128) {
		int a = num % 128;
		bitset<8> byte(a);
		byte.flip(7);
		num = (num - a) / 128;
		b = (uint8_t)byte.to_ulong();
		//std::cout << byte << endl;
		result++;
		writer->SetStruct(b);

	}
	int a = num % 128;
	bitset<8> byte(a);
	//std::cout << byte << endl;
	b = (uint8_t)byte.to_ulong();
	result++;
	writer->SetStruct(b);
	return result;
}

int VBDecode(MyFileReader<char>* reader)
//uncompress a integer from VAR BYTE from the file
{
	char c;
	int num, p;

	char* t = reader->GetStruct();
	if (t == "" || t == NULL)return -1;
	c = *t;
	delete t;

	bitset<8> byte(c);
	num = 0;
	p = 0;
	while (byte[7] == 1) {
		byte.flip(7);
		num += (int)(byte.to_ulong()*pow(128, p));
		p++;
		char* q = reader->GetStruct();
		c = *q;
		delete q;
		byte = bitset<8>(c);
	}
	num += (int)(byte.to_ulong()*pow(128, p));

	return num;
}

int ReadTermID(MyFileReader<char>* reader)
{
	return VBDecode(reader);
}

int WriteTermID(MyFileWriter<char>* writer, int termID)
{
	return VBEncode(writer, termID);
}