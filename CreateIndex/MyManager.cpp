#include "pch.h"
#include "MyManager.h"


MyManager::MyManager()
{
	InitializeMinHash();
	
}

MyManager::~MyManager()
{
}

void MyManager::changeMode(int n)
{
	mode = n;
}

void MyManager::InitializeMinHash()
{
	srand((unsigned)time(NULL));

	for (int i = 0; i < MinHashSize; i++)
	{//rand=0-32767
		int a = rand();
		minHashArgs[i].first = rand() * 371 + (rand() % (371 - 1) + 1);
		minHashArgs[i].second = rand() * 371 + (rand() % (371 - 1) + 1);
	}
}

int MyManager::MinHash(int x, int No)
{
	return (minHashArgs[No].first *x + minHashArgs[No].second) % 12255871;
}

void MyManager::GetFiles(string path)
//put all the files under path directory into a list called vsfiles
{
	//initialization
	iLoadFilenum = 0;
	vsfiles.clear();

	//get all the files names
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))//if another directory, loop
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetFiles(p.assign(path).append("\\").append(fileinfo.name));
			}
			else//if a file, put into vsfiles(a vector contains type string)
			{
				vsfiles.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

string MyManager::LoadFile()
//get the name of next unread file. if no file left, returns ""
{
	string result = "";
	if (iLoadFilenum < (int)vsfiles.size())
	{
		result = vsfiles[iLoadFilenum];
		iLoadFilenum++;
	}
	return result;
}

bool MyManager::Deduplicate()
//read all the pages, put postings into a heap until it is full, then output this heap and loop
{	
	//initialize file reader

	if (vsfiles.size() == 0)
	{
		cout << "no files in pages\\"<< endl;
		return false;
	}
	Clear_resultSimHash();

	int iPageWordCount = 0;//used to count position
	int iPageGoodWordCount = 0;

	//InvertedIndex.DocClear();

	string sReadTemp;
	while ((sReadTemp = LoadFile()) != "")//if some files remained
	{
		MyBufferReader* page_reader = new MyBufferReader(sReadTemp);

		//initialize
		int iState = -1;
		//some kind of state machine to match the pages
		//-1 before the first useful page
		//0 after "WARC/1.0" 
		//1 after "WARC-Target-URI:"
		//2 after actual URL
		//3 after "Content-Length:"
		//4 after length value
		bool isFirst = true;// to skip the first "WARC/1.0"

		string sWord;
		while ((sWord = page_reader->GetOneWord()) != "")
		{
			//start parser
			if (sWord == "WARC/1.0")
			{
				if (isFirst)
				{
					isFirst = false;
				}
				else
				{
					if (iState == -1 || iState == 4)//if a new doc
					{
						if (iState == 4)//not first
						{
							Output_resultSimHash(iPageGoodWordCount, iPageWordCount);

							//InvertedIndex.SetDocLength(iComLenCount - InvertedIndex.GetNowDocStart());
						}
						//InvertedIndex.NextDoc();
						//InvertedIndex.SetDocStart(iComLenCount);

						iState = 0;
						iPageWordCount = 0;
						iPageGoodWordCount = 0;
					}
				}
			}
			else if (sWord == "WARC-Target-URI:")
			{
				if (iState == 0)
				{
					iState = 1;
				}
			}
			else if (sWord == "Content-Length:")
			{
				if (iState == 2)
				{
					iState = 3;
				}
			}
			else
			{
				switch (iState)
				{
				case 1:
				{
					//record url
					MeetURL(sWord);

					//InvertedIndex.SetDocUrl(sWord);
					iState = 2;
					break;
				}
				case 3:
				{
					//InvertedIndex.SetDocSize(stoi(sWord));
					iState = 4;
					break;
				}
				case 4:
				{
					if (!isFirst)
					{						
						if (isEnglish(sWord))//is english word
						{
							//handle a term
							MeetTerm(sWord);

							iPageGoodWordCount++;
						}
						iPageWordCount++;
					}
					break;
				}
				default:
					break;
				}
			}
		}
		delete page_reader;
	}
	//InvertedIndex.SetDocLength(iComLenCount - InvertedIndex.GetNowDocStart());
	Output_resultSimHash(iPageGoodWordCount, iPageWordCount);



	return true;
}


bool MyManager::isEnglish(string& word)
{
	int len = (int)word.length();
	for (int i = 0; i < len; i++)
	{
		if ((word[i] >= 'A' && word[i] <= 'Z'))
		{
			word[i] += 32;
		}
		else if ((word[i] >= 'a' && word[i] <= 'z'))
		{
			continue;
		}
		else if (i == len - 1 && (word[i] == '.' || word[i] == ','))
		{
			word.erase(i, 1);
			if (word == "")return false;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void MyManager::MeetURL(string URL)
{
	if (mode == 1)//min hash
	{
		MyDocument_min tempdoc;
		tempdoc.URL = URL;
		tempdoc.isBad = false;
		for (int i = 0; i < MinHashSize; i++)
		{
			tempdoc.signature[i] = INT_MAX;
		}
		documentlist_min.push_back(tempdoc);
	}
	else//sim hash
	{
		MyDocument_sim tempdoc;
		tempdoc.URL = URL;
		tempdoc.isBad = false;
		tempdoc.value = 0;
		documentlist_sim.push_back(tempdoc);
	}
}

void MyManager::MeetTerm(string term)
{
	if (mode == 1)//min hash
	{
		//get term ID
		int termID = 0;
		int num = termIDMap.size();
		if (termIDMap[term] == 0)
		{
			termIDMap[term] = num + 1;
			termID = num + 1;
		}
		else
		{
			termID = termIDMap[term];
		}

		//compute signature
		int docID = documentlist_min.size() - 1;
		for (int i = 0; i < MinHashSize; i++)
		{
			int mhresult = MinHash(termID, i);
			if (mhresult < documentlist_min[docID].signature[i])
			{
				documentlist_min[docID].signature[i] = mhresult;
			}
		}
	}
	else//sim hash
	{
		//get hash value
		size_t hash_result1 = FNV1A_64_HASH(term);
		size_t hash_result2 = hash<string>{}(term);
		int weight = StrToWeight(term);
		//int weight = 1;

		//add value(per hash bit) to each bit
		for (int i = SimHashSize - 1; i >= SimHashSize / 2; i--)
		{
			if (hash_result1 & 1)
			{
				resultSimHash[i] += weight;
			}
			else
			{
				resultSimHash[i] -= weight;
			}
			hash_result1 = hash_result1 >> 1;
		}
		for (int i = SimHashSize / 2 - 1; i >= 0; i--)
		{
			if (hash_result2 & 1)
			{
				resultSimHash[i] += weight;
			}
			else
			{
				resultSimHash[i] -= weight;
			}
			hash_result2 = hash_result2 >> 1;
		}
	}

}

void MyManager::Output_resultSimHash(int good, int total)
{
	//convert 64-bit into document value
	if (mode == 1)
	{
		int docid = documentlist_min.size() - 1;
		if (good * 1.0 / total < 0.8)
		{
			documentlist_min[docid].isBad = true;
			return;
		}
		for (int i = 0; i < BandNumber; i++)
		{
			string temp = "";
			for (int j = i * BandSize; j < (i + 1)*BandSize; j++)
			{
				temp += to_string(documentlist_min[docid].signature[j]);
			}
			bm[i].InsertBand(docid, temp, true);
		}
	}
	else 
	{
		int docid = documentlist_sim.size() - 1;
		if (good * 1.0 / total < 0.8)
		{
			documentlist_sim[docid].isBad = true;
			return;
		}
		uint64_t result = 0;
		string temp = "";
		for (int i = 0; i < SimHashSize; i++)
		{
			result = result << 1;
			if (resultSimHash[i] > 0)
			{
				temp += "1";
				result = result | 1;
			}
			else
			{
				temp += "0";
			}
		}
		documentlist_sim[docid].value = result;
		Clear_resultSimHash();

		for (int i = 0; i < BandNumber; i++)
		{
			string temp = "";
			int bandsize = 16; // SimHashSize / BandNumber;
			int nowvalue = (documentlist_sim[docid].value >> bandsize * (BandNumber - 1 - i)) & 0xffff;

			temp = to_string(nowvalue);
			bm[i].InsertBand(docid, temp, false);
		}
	}
}

void MyManager::Clear_resultSimHash()
{
	for (int i = 0; i < SimHashSize; i++)
	{
		resultSimHash[i] = 0;
	}
}


void MyManager::StartHash()
{
	string soutfile = "";
	if (mode == 1)
	{
		soutfile = "result/minhashresult.txt";
	}
	else
	{
		soutfile = "result/simhashresult.txt";
	}
	MyBufferWriter * bw = new MyBufferWriter(soutfile);
	int dsize = 0;
	float scorenum = 0.0;
	if (mode == 1)
	{
		dsize = documentlist_min.size();
	}
	else
	{
		dsize = documentlist_sim.size();
	}
	string stemp = "total file number:" + to_string(dsize) + "\n";
	//cout << stemp << endl;
	bw->SetOneWord(stemp);

	//new method
	for (int i = 0; i < BandNumber; i++)
	{
		cout << "\t\nband " << i <<endl;
		ShowBucket(i,bw);
	}

	//over
	bw->RefreshBuffer();
	delete bw;
}


void MyManager::ShowBucket(int bucketNO, MyBufferWriter * bw)
{
	for (int i = 0; i < SimHashSize; i++)
	{
		metascore[i] = 0;
	}
	float scorenum = 0.0;

	unordered_map<int, DocumentPointer*>::iterator iter;
	for (iter = bm[bucketNO].hashtable.begin(); iter != bm[bucketNO].hashtable.end(); iter++)
	{
		string result = "bucket " + to_string(iter->first) + "\n";

		DocumentPointer* temp = iter->second;
		vector<int> BucketDocs;

		while (temp)
		{
			BucketDocs.push_back(temp->DocID);

			temp = temp->pointer;
		}

		if (BucketDocs.size() > 1)
		{
			for (int i = 0; i < BucketDocs.size(); i++)
			{
				for (int j = i + 1; j < BucketDocs.size(); j++)
				{
					int r = CompareDoc(BucketDocs[i], BucketDocs[j]);
					if (r > 0)
					{
						//if(mode==2)documentlist_sim[j].value = 0;
						if (mode == 1)
						{
							string stemp = "file " + documentlist_min[BucketDocs[i]].URL + " and " + documentlist_min[BucketDocs[j]].URL + " score: " + to_string(r) + "\n";
							//cout << stemp << endl;
							bw->SetOneWord(stemp);
						}
						else
						{
							string stemp = "file " + documentlist_sim[BucketDocs[i]].URL + " and " + documentlist_sim[BucketDocs[j]].URL + " score: " + to_string(r) + "\n";
							//cout << stemp << endl;
							bw->SetOneWord(stemp);
						}

						metascore[r - 1]++;
						scorenum++;
					}

				}
			}
		}

	}

	for (int i = 0; i < SimHashSize; i++)
	{
		if (metascore[i] != 0)cout << "score " << to_string(i + 1) << " portion: " << to_string(metascore[i] / scorenum) << " number: " << to_string(metascore[i]) << endl;
	}
}

int MyManager::CompareDoc(int x, int y)//return similarity between two documents x and y
{
	int result;
	if (mode == 1)//min hash
	{
		if (documentlist_min[x].isBad || documentlist_min[y].isBad) return 0;
		//result equals the number of same signatures between two documents
		result = MinHashSize;
		for (int i = 0; i < MinHashSize; i++)
		{
			if (documentlist_min[x].signature[i] != documentlist_min[y].signature[i])
			{
				result--;
				if (result < MinHashThreshold)
				{
					result = 0;
					break;
				}
			}
		}
	}
	else//sim hash
	{
		if (documentlist_sim[x].isBad || documentlist_sim[y].isBad) return 0;
		//result equals the number of identical bits between two documents
		result = SimHashSize;
		if (documentlist_sim[x].value == 0 || documentlist_sim[y].value == 0)
		{
			return 0;
		}
		uint64_t compareValue = documentlist_sim[x].value ^ documentlist_sim[y].value;//xor operation
		for (int i = SimHashSize - 1; i >= 0; i--)
		{
			if (compareValue & 1)//one difference
			{
				result--;
				if (result < SimHashThreshold)
				{
					result = 0;
					break;
				}
			}
			compareValue = compareValue >> 1;
		}
	}
	return result;
}





int MyManager::StrToWeight(string word)
//Length-Frequency Statistics for Written English
//chrome-extension://oemmndcbldboiebfnladdacbdfmadadm/https://core.ac.uk/download/pdf/82753461.pdf
{
	int len = word.length();
	/*
	switch (len)
	{
	case 3:
		return 5;
		break;
	case 4:
	case 2:
		return 4;
		break;
	case 5:
		return 3;
		break;
	case 6:
	case 7:
		return 2;
		break;
	default:
		return 1;
		break;
	}
	*/
	switch (len)
	{
	case 1:
	case 2:
		return 1;
		break;
	case 3:
		return 2;
		break;
	case 4:
	case 5:
		return 3;
		break;
	case 6:
		return 4;
		break;
	default:
		return 5;
		break;
	}
}

long MyManager::FNV1A_64_HASH(string k)
{
	long rv = 0xcbf29ce484222325L;
	long FNV_64_PRIME = 0x100000001b3L;
	int len = k.length();
	for (int i = 0; i < len; i++) {
		rv ^= k[i];
		rv *= FNV_64_PRIME;
	}
	return rv;
}

