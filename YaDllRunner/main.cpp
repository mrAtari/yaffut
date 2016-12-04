// YaDllRunner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define NOGDI
#define NOUSER
#define NOKERNEL
#define NOSOUND
#define NOMINMAX
#include <Windows.h>


#include <iostream>

using namespace std;

#include "..\Yaffut\yaffut.h"

typedef int (*runTests)(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
	if (argc == 1)
	{
		//no name for Test dll given
		cout << "no name for test dll given";
		return 1;
	}

	HINSTANCE inst = ::LoadLibraryA(argv[1]);
	
	if (inst == NULL)
	{
		cout << "Can't load given dll";
		return 2;
	}

	try
	{
		argc = 1; //ignore arguments
		runTests symbol = (runTests)::GetProcAddress(inst, YAFFUT_TEST_NODE);
	
		(*symbol)(argc, argv);
	}
	catch (...)
	{
		cout << "Test throws an exception";
	}
	::FreeLibrary(inst);

	return 0;
}

