#include "pch.h"
#include "MyBufferWriter.h"


MyBufferWriter::MyBufferWriter()
{
}

MyBufferWriter::MyBufferWriter(string filename)
{
	out_file.open(filename, ios::out | ios::binary);
	buffer_num = 0;
	buffer_point = 0;
}

MyBufferWriter::~MyBufferWriter()
{
	out_file.close();
}

void MyBufferWriter::RefreshBuffer()
//refresh buffer
{
	//cout << "write refresh" << endl;
	out_file.write(buffer, buffer_point);
	out_file << endl;
	buffer_point = 0;
}

void MyBufferWriter::SetOneWord(string word)
//Each time calling this function, you can set a word into buffer 
//if buffer meets its end, this function will automatically refresh the buffer
//caution: if you want to use this function, dont forget to call RefreshBuffer() to put buffer into the file
{
	if ((int)word.length() + buffer_point < iBufferSize)//if buffer meets its end
	{
		strcpy_s(&buffer[buffer_point], iBufferSize-buffer_point,(word).c_str());//put the word into buffer
		buffer_point += word.length();
	}
	else
	{
		RefreshBuffer();//write buffer into file
		SetOneWord(word);//set the word into new buffer
	}
}
