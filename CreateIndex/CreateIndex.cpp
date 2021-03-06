#include "pch.h"
#include "time.h"
#include "MyManager.h"
#include "MyBufferReader.h"
#include <iostream>
#include <string>


#include <functional>
using namespace std;
clock_t start, finish;
double totaltime;
void showTime();




int main()
{
	start = clock();	

	MyManager manager;
	cout << "select your hash mode(1 minhash 2 simhash)" << endl;
	int mode = 0;
	cin >> mode;
	if (mode == 1)
	{
		manager.changeMode(1);
	}
	else if(mode==2)
	{
		manager.changeMode(2);
	}
	else
	{
		return 0;
	}
	manager.GetFiles("pages");
	manager.Deduplicate();
	manager.StartHash();
	showTime();
	cout << "program finish!" << endl;
}

void showTime()
{
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\nprogram running time: " << totaltime << "s！" << endl;
	start = clock();
}


