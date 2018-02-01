#include "stdafx.h"
#include "MzParser.h"

/* Variables for the Maze Parser program*/
const char  delims[] = " "; //  ,\t\n
std::string mazeFile;

MzParser::MzParser(std::string mazeFileIn)
{
	printf("Loading Maze %s ", mazeFileIn.c_str());
	mazeFile = mazeFileIn;
	cntTextureRead = 0;
	cntFloorPlanRead = 0;
	cntTexture = 0;
	cntCellSize = 0;
	cntWallHeight = 0;
	dimSceneX = 0;
	dimSceneY = 0;
}


MzParser::~MzParser()
{
}

void MzParser::readTextureFromConfigLine(char* config_line) {
	char* context = NULL;
	char* token = strtok_s(config_line, delims, &context);
	if (token != NULL) {
		printf("Id: %s\t", token);
		token = strtok_s(NULL, delims, &context);
		printf("Tex Name: %s, Len: %d\t ", token, stringLength(token));
		texturesList.push_back(sconvert(token, stringLength(token)));
	}
}

/* evaluate the string length */
int MzParser::stringLength(char *s) {
	int c = 0;
	while (s[c] != '\0')
		c++;
	return c;
}

std::string MzParser::sconvert(const char *pCh, int arraySize) {
	std::string str;
	if (pCh[arraySize - 1] == '\0') str.append(pCh);
	else 
		for (int i = 0; i < arraySize; i++) {
			if(pCh[i] != '\n')
				str.append(1, pCh[i]);
		}
	printf(" Str: %s ", str.c_str());
	return str;
}

/* Read various count and sizes from the configuration line buffer */
void MzParser::readCountFromConfigLine(char* config_line) {
	char* context = NULL;
	char* token = strtok_s(config_line, delims, &context);
	if (token != NULL) {
		printf("Config Name: %s\t", token);
		if (strstr(token, "CELL")) {
			token = strtok_s(NULL, delims, &context);
			if (token != NULL) sscanf_s(token, "%d", &cntCellSize);
		}else if (strstr(token, "HEIGHT")) {
			token = strtok_s(NULL, delims, &context);
			if (token != NULL) sscanf_s(token, "%d", &cntWallHeight);
		}else if (strstr(token, "TEXTURES")) {
			token = strtok_s(NULL, delims, &context);
			if (token != NULL) sscanf_s(token, "%d", &cntTexture);
		}
		printf("Value: %d\n", atoi(token));
	}
}

/* Read dimensions from the configuration line buffer */
void MzParser::readDimsFromConfigLine(char* config_line) {
	char* context = NULL;
	char* token = strtok_s(config_line, delims, &context);
	if (token != NULL) {
		printf("Dim: %s\t", token);
		token = strtok_s(NULL, delims, &context);
		if (token != NULL) sscanf_s(token, "%d", &dimSceneX);
		token = strtok_s(NULL, delims, &context);
		if (token != NULL) sscanf_s(token, "%d", &dimSceneY);
		printf("X,Y: %d,%d\n", dimSceneX, dimSceneY);
	}
}

/* Read the floor plan information from the read config line buffer */
void MzParser::readFloorPlanFromConfigLine(char* config_line) {
	char* context = NULL;
	char* token = strtok_s(config_line, delims, &context);
	int planVal = 0;
	if (mazeFloorPlan == NULL) { 
		printf("Initialized the Maze plan for size %dx%d \n", dimSceneX, dimSceneY);
		//Maze Frame buffer rows * 2 (Vertical info)
		mazeFloorPlan = (int **)malloc(sizeof(int*) * dimSceneY * 2);
		for (int i = 0; i <= (dimSceneY * 2); i++) { // for each row
			mazeFloorPlan[i] = (int *)malloc(sizeof(int) * dimSceneX);
		}
	}
	int j = 0;
	printf("Plan %d :\t", cntFloorPlanRead-1);
	// While there are any tokens left in "config_line"
	while (token != NULL){
		if (token[0] == '#' || strstr(token, "#")) {
			break;
		}
		sscanf_s(token, "%d", &planVal);
		mazeFloorPlan[(cntFloorPlanRead - 1)][j] = planVal;
		token = strtok_s(NULL, delims, &context);
		j++;
		printf(" %d", planVal);
	}
	printf("\n");
}

/* Create the frame buffer from the maze floorplan */
void MzParser::createMazeFramebuffer(unsigned char fBuf[window_height][window_width][RGBSIZE]) {
	printf("Maze size is %dx%d window\n", dimSceneX, dimSceneY);
	// Index for starting position & offset for unit increment
	int cellRow = 0, cellCol = 0;
	int x1PxId = 0, x2PxId = 0, xOffset = (window_width / dimSceneX);
	int y1PxId = 0, y2PxId = 0, yOffset = (window_height / dimSceneY);
	printf("Frame buffer for %dx%d window\n", window_height, window_width);
	printf("Cell pixel offset (0,0)->(%dx%d) in window\n", xOffset, yOffset);
	int brush_size = 2;
	int i, j, textureId, c = 255; // Color the frame buffer white
	for (i = 0; i < (dimSceneY * 2); i++) {				// Row - Height
		for (j = 0; j < dimSceneX; j++, cellCol++) {	// Col - Width
			textureId = mazeFloorPlan[i][j];
			if (i % 2 == 0) {
				if (textureId > 0) {
					printf("H(%d,%d.) Draw horizontal lines\n", cellRow, cellCol);
					x1PxId = j * xOffset;
					x2PxId = x1PxId + xOffset;
					y1PxId = cellRow * yOffset;
					y2PxId = y1PxId + brush_size;
					fillUnitCellFrameBuffer(fBuf, x1PxId, window_height - y1PxId,
												  x2PxId, window_height - y2PxId, textureId);
				}else
					if (j == 0) printf("%d. Empty horizontal lines\n", cellRow);

			} else {
				if (textureId > 0) {
					printf("V(%d,%d.) Draw vertical lines\n", cellRow, cellCol);
					x1PxId = j * xOffset;
					x2PxId = x1PxId + brush_size;
					y1PxId = cellRow * yOffset;
					y2PxId = y1PxId + yOffset;
					fillUnitCellFrameBuffer(fBuf, x1PxId, window_height - y1PxId,
												  x2PxId, window_height - y2PxId, textureId);
				}else
					if (j == 0) printf("%d. Empty vertical lines\n", cellRow);
			}
			if (cellCol == (dimSceneX - 1)) cellCol = 0; // Reset cellCol
		}
		// Increment row after the first odd call
		if (i % 2 != 0) cellRow++;
	}
	printf("Framed\n");
}

/* 
 * Fill the rect in the frame buffer with color 
 * Two sets of triangle will be prepared and listed
 */
void MzParser::fillUnitCellFrameBuffer(unsigned char fBuf[window_height][window_width][RGBSIZE], 
										int x1, int y1, int x2, int y2, int textureId) {
	for (int i = x1; i <= x2 && i >= 0; i++) {
		for (int j = y2; j < y1 && j >= 0; j++)
		{
			fBuf[j][i][0] = textureId;  // Black out
			fBuf[j][i][1] = textureId * 30;
			fBuf[j][i][2] = textureId;
		}
	}
}

/* Read Maze floor plan from the maze file*/
int MzParser::readMaze() {
	int count = 0;
	errno_t err;
	printf("Reading Maze file\n");
	err = fopen_s(&fpMz, mazeFile.c_str(), "r");
	if (err == 0) {
		/* then file opened successfully. */
		while (fgets(bufr, MAXLINE, fpMz) != NULL) {
			if (bufr[0] == '#' || strlen(bufr) < 4) {
				continue;
			}
			if (strstr(bufr, "TEXTURES") || cntTextureRead > 0) {
				if (cntTextureRead > 0) {
					// then read texture files
					readTextureFromConfigLine(bufr);
					if (cntTextureRead == cntTexture)
						cntTextureRead = 0; // Reset Texture read
					else 
						cntTextureRead++;
				}else{
					// Find Texture section
					readCountFromConfigLine(bufr);
					cntTextureRead++;
				}
			}else if (strstr(bufr, "CELL")) {
				readCountFromConfigLine(bufr);
			}else if (strstr(bufr, "HEIGHT")) {
				readCountFromConfigLine(bufr);
			}else if (strstr(bufr, "DIMENSIONS")) {
				readDimsFromConfigLine(bufr);
			}else if (strstr(bufr, "FLOORPLAN") || cntFloorPlanRead > 0) {
				if (strstr(bufr, "END")) {
					cntFloorPlanRead = 0; // Stop if found END
					continue;
				}
				if (cntFloorPlanRead > 0) {
					readFloorPlanFromConfigLine(bufr);
				}
				cntFloorPlanRead++; // Start Floor Plan read
			}
			/* then no read error */
			count += 1;
		}
		if (err != 0) {
			//exit(1);
		}
	}
	else {                    /* there was an error on open */
		printf("Error in opening file \n");
	}
	
	return 1;
}

/* Print all the configurations */
void MzParser::printConfigurations() {
	printf("Dimensions	: %d x %d \n", dimSceneX, dimSceneY);
	printf("Wall Height	: %d \n", cntWallHeight);
	printf("Cell Size	: %d \n", cntCellSize);
	printf("Textr Count	: %d \n", cntTexture);
	for (size_t i = 0; i < texturesList.size(); ++i)
		printf("%d. Texture: %s \n", i, texturesList[i].c_str());
}

/* Test memory allocation in a class*/
void MzParser::testAlloc() {

	// Vector push
	//vestring.push_back("A String");

	// Char array 
	char *ptr = NULL;
	int i;
	ptr = (char *)malloc(sizeof(char) * 30);
	for (i = 0; i<30; i += 6)
		gets_s(&ptr[i], 30);
	for (i = 0; i<30; i += 6)
		puts(&ptr[i]);
	getchar();
}
