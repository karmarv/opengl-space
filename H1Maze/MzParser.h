#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 100

class MzParser
{
public:
	MzParser();
	virtual ~MzParser();

	int readMaze();
};

