#include "pch.h"
#include "MyBufferReader.h"



MyBufferReader::MyBufferReader()
{
	cBufferPoint = NULL;
}

MyBufferReader::MyBufferReader(string filename)
{
	MyBufferReader();
	infile.open(filename, ios::in | ios::binary);
	iReadEnd = -1;
}

MyBufferReader::MyBufferReader(string filename,int iTermSart, int iTermEnd)
{
	infile.open(filename, ios::in | ios::binary);
	infile.seekg(iTermSart, ios::beg);
	iReadPos = iTermSart;
	iReadEnd = iTermEnd;

	RefreshBuffer();
}

MyBufferReader::~MyBufferReader()
{
	infile.close();
}


bool MyBufferReader::RefreshBuffer()
//refresh the buffer, if there is no more new content from file, it returns false
{
	cBufferPoint = NULL;

	infile.read((char*)buffer, sizeof(char)*bufferSize);//fill in buffer

	__int64 iGcount = infile.gcount();//means number of successfully readed words
	isBufferSpaceStart = (buffer[0] == ' ' || buffer[0] == '\n' || buffer[0] == '\r');
	isBufferSpaceEnd = (buffer[iGcount - 1] == ' ' || buffer[iGcount - 1] == '\n' || buffer[iGcount - 1] == '\r');
	//if (iGcount < sizeof(buffer))//if buffer is not full, add '\0' to seperate new content and the old
	buffer[iGcount] = NULL;

	if (iGcount != 0)
	{
		//cout << "read refresh" << endl;
		return true;
	}
	else
	{
		return false;
	}
}

string MyBufferReader::GetOneWord()
//Each time calling this function, you can get a complete word from buffer by seperating terms " \r\n" 
//if buffer meets its end, this function will automatically refresh the buffer and concatenate the last word and the first word.
//if this function returns "", it means the file meets its end, this MyBufferReader finishes its job.
{
	string result = "";
	const char * seperater = " \r\n";
	char* strToken = NULL;
	if (iReadEnd==-1 || iReadPos < iReadEnd)
	{
		if (cBufferPoint == NULL)//if it is a new buffer
		{
			strToken = strtok_s(buffer, seperater, &cBufferPoint);//get a word from buffer and record the pointer
			if (iReadEnd != -1)
			{
				iReadPos += (cBufferPoint - &buffer[0]);
			}
		}
		else
		{
			char* bufferTemp = cBufferPoint;
			strToken = strtok_s(cBufferPoint, seperater, &cBufferPoint);//get a word from last pointer and record the new pointer
			if (iReadEnd != -1)
			{
				iReadPos += (cBufferPoint - bufferTemp);
			}
		}

		if (strToken)//if get a word
		{
			result = strToken;
			if (*cBufferPoint == '\0')//buffer meets its end
			{
				bool isBSE = isBufferSpaceEnd;
				if (RefreshBuffer())//refresh buffer successfully
				{
					if (!isBufferSpaceStart && !isBSE)//if there is no space between the last buffer and the current buffer
					{
						result += GetOneWord();//concatenate the last word and the first word
					}
				}
			}
		}
		else
		{
			if (RefreshBuffer())//need to refresh buffer
			{
				result = GetOneWord();
			}
			//if refreshing buffer not successfully, result will remain "" which means function fails
		}


	}
	return result;
}
