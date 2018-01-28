#include "stdafx.h"
#include "MzParser.h"

FILE * fp;
char bufr[MAXLINE];

extern int errno;

MzParser::MzParser()
{
}


MzParser::~MzParser()
{
}



void readStrFromConfigLine(char* config_line) {
	char prm_name[MAXLINE];
	sscanf_s(config_line, "%s", prm_name);
	printf("Read: %s",prm_name);
}


int MzParser::readMaze() {
	int count = 0;
	errno_t err;
	printf("Reading Maze file\n");
	err = fopen_s(&fp, "assets/maze2.txt", "r");
	if (err == 0) {
		/* then file opened successfully. */
		while (fgets(bufr, MAXLINE, fp) != NULL) {
			if (bufr[0] == '#' || strlen(bufr) < 4) {
				continue;
			}
			if (strstr(bufr, "FLOORPLAN")) {
				readStrFromConfigLine(bufr);
			}

			/* then no read error */
			count += 1;
			printf("%d: %s",     /* no "\n", why? */
				count, bufr);
		}
		/* fgets returned null */
		if (errno != 0) {
			//exit(1);
		}
		
	}
	else {                    /* there was an error on open */
		printf("Error in opening file \n");
	}
	getchar();
	return 1;
}
