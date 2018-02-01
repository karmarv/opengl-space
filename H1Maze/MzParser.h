#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "globals.h"


class MzParser
{
public:
	/************************** VARIABLES ********************************/

	// Maze floor plan matrix
	int** mazeFloorPlan = NULL;
	std::vector<std::string> texturesList;

	// Triangle Vertices for texture
	int **triangleVertices;
	
	// Configuration Variables
	FILE * fpMz;
	char bufr[MAXLINE];
	int cntTexture, cntCellSize, cntWallHeight; // Configs
	int dimSceneX, dimSceneY;
	int cntTextureRead, cntFloorPlanRead;			// Status

	/************************** FUNCTIONS ********************************/

	MzParser(std::string mazeFileIn);
	virtual ~MzParser();

	int readMaze();
	void readCountFromConfigLine(char* config_line);
	void readDimsFromConfigLine(char* config_line);
	void readTextureFromConfigLine(char* config_line);
	void readFloorPlanFromConfigLine(char* config_line);
	int stringLength(char *s);
	std::string sconvert(const char *pCh, int arraySize);
	void printConfigurations();
	void testAlloc();
	
	void createMazeFramebuffer(unsigned char frameBuffer[window_height][window_width][RGBSIZE]);
	void fillUnitCellFrameBuffer(unsigned char frameBuffer[window_height][window_width][RGBSIZE], int x1, int y1, int x2, int y2, int color);
};

