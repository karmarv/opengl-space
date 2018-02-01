
/*
* -----------------------------------------------------------------------------
* Author	: Rahul Vishwakarma
* Date		: 24 January 2018
* Purpose	: Drawing program using OpenGL, to read/write/manipulate image files
* -----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <windows.h>
#include "stdafx.h"

#define cimg_use_tiff

#include <gl/glut.h>	//  Include GLUT, OpenGL, and GLU libraries
#include "CImg.h"		//  CImg C Library
#include "globals.h"

using namespace cimg_library;

#include "MzParser.h"	// Maze parser utility




/************************** VARIABLES ********************************/

/* Variables */
GLuint *textureIds;
int activeTextureId = 0;
int textureCount = 0;

char fileFullPath[255];
//CImg<unsigned char> *image = NULL;
GLubyte checkImage[window_height][window_width][3]; // Static allocation of frame buffer
													//GLubyte ***checkImage = NULL;
GLubyte color[] = { 0, 0, 0, 255 };
int brush_size = 1;

MzParser *mzParser = NULL;


// Action variables


GLfloat rotationX = 0.0f, rotationY = 0.0f;
GLfloat mvX = 0.0f, mvY = 0.0f;
/************************** FUNCTIONS ********************************/

/* Initialize the framebuffer with white color */
void makeCheckImage(GLubyte color[])
{
	brush_size = 1;
	int i, j, c;
	for (i = 0; i < window_height; i++) {
		for (j = 0; j < window_width; j++) {
			//Color the frame buffer
			c =  ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) ;
			checkImage[i][j][0] = (GLubyte)c * color[0];
			checkImage[i][j][1] = (GLubyte)c * color[1];
			checkImage[i][j][2] = (GLubyte)c * color[2];
		}
	}
}



/* Set the color on a particular pixel in frame buffer */
void setPixel(int x, int y) {

	int x1 = x - brush_size,
		y1 = window_height - (y - brush_size),
		x2 = x + brush_size,
		y2 = window_height - (y + brush_size);
	for (int i = x1; i <= x2; i++)
	{
		for (int j = y2; j <= y1 && j >= 0; j++)
		{
			checkImage[j][i][0] = (GLubyte)color[0];
			checkImage[j][i][1] = (GLubyte)color[1];
			checkImage[j][i][2] = (GLubyte)color[2];
		}
	}
}

/**********************************************************/

/* this function checks for GL errors that might have cropped up and
breaks the program if they have */
void checkGLErrors(char *prefix) {
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "%s: found gl error: %s\n", prefix, gluErrorString(error));
		exit(-1);
	}
}

/* shape the view port upon redisplay */
void reshape(int w, int h)
{
	// Compute aspect ratio of the new window
	if (h == 0) h = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)w / (GLfloat)h;

	// Set the viewport to cover the new window
	glViewport(0, 0, w, h);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset
	// Enable perspective projection with fovy, aspect, zNear and zFar
	//Set the projection matrix to a perspective with a 45 degree vertical field of view with an aspect ratio of w/h, the near-clip plane 1 unit away, and the far-clip plane 100 units away.
	gluPerspective(60.0f, aspect, 0.1f, 1000.0f); 
	glMatrixMode(GL_MODELVIEW);
	
	//match projection to window resolution (could be in reshape callback)
	//glOrtho(0, window_width, 0, window_height, -1, -50);
	//glTranslatef(0.0f, 0.0f, -3.6f);



}


/* Initialize the state and buffers */
void init(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
							   //	glEnable(GL_CULL_FACE);
							   //	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	//help();
}

/* Draw floor on in the world space */
void drawFloor() {
	// Provide the right texture for the floor, since it is last
	glBindTexture(GL_TEXTURE_2D, textureIds[textureCount -1]);
	glBegin(GL_TRIANGLES);
	float size = mzParser->cntCellSize * mzParser->dimSceneX, hlfsz = size/2;
	//printf("Draw floor with edge: %f\n",size);
	// Say hard coding dims 5x5, with cell size 10
	// Floor triangle 1 (center of the world (50x50) as the origin)
	glTexCoord2f(0.0, 0.0); glVertex3f(-hlfsz, -hlfsz, 0.0);
	glTexCoord2f(0.0, 1.0);	glVertex3f(-hlfsz,  hlfsz, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(hlfsz, -hlfsz, 0.0);

	glTexCoord2f(0.0, 1.0);	glVertex3f(-hlfsz, hlfsz, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(hlfsz, hlfsz, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(hlfsz, -hlfsz, 0.0);
	glEnd();
}

/* Opengl draw the framebuffer on the display */
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	char error_msg[25] = "Errors in display()!\n";
	checkGLErrors(error_msg);


	// Transform World space to eye space
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
	glLoadIdentity();
	glPushMatrix();
	// Eye (-200, -200, 50), World (0,0,0), 
	
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glPopMatrix();


	// Transform from eye space to 2D window perspective
	// gluPerspective




	
	// Rotation first & then translate
	//glPushMatrix();
	//glLoadIdentity();// Reset transformations
	glRotatef(rotationX, 1.0, 0.0, 0.0);// Rotate when user changes rotate_x and rotate_y
	glRotatef(rotationY, 0.0, 1.0, 0.0);
	glTranslatef(mvX, mvY, -7.0f);  // Move left and into the screen
	//glPopMatrix();
	//glLoadIdentity();
	//

	//Render 
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	// Draw floor with a texture
	drawFloor();

	glDisable(GL_TEXTURE_2D);


	glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

/* Opengl redisplay function */
void redisplay(void) {
	glutPostRedisplay();
	return;
}

/* Instructions for the program functions */
void help() {
	printf("-------------------------------------\n");
	printf("Keyboard commands for the application\n");
	printf("-------------------------------------\n\n");
	printf("\t 1. Quit \t\t: q/Q \n");
	printf("\t 2. Image \t\t: i/I \n");
	printf("\t 3. Clear \t\t: Space bar \n");
	printf("\t 4. Save \t\t: k/K \n");
	printf("\n");
	printf("\t 5. Fill Red \t\t: 1-3 \n");
	printf("\t 6. Fill Green \t\t: 4-6 \n");
	printf("\t 7. Fill Blue \t\t: 7-9 \n");
	printf("\t 8. Fill Black \t\t: 0 \n");
	printf("\t 9. Brush Size \t\t: b/B \n");
	printf("\n");
	printf("\t 10. Help \t\t: h/H \n\n");
	printf("-------------------------------------\n");
}

/* Keyboard key down events */
void keyboard(unsigned char key, int x, int y) {
	int c = 0;
	GLfloat rotationSpeed = 10.0f;
	GLubyte col[] = { 255, 255, 255 };
	// Key press case logic
	switch (key) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			c = key - '0';
			//Set Colors and reset color on '0'
			if (c == 0) {
				color[0] = 0;
				color[1] = 0;
				color[2] = 0;
			}
			else if (c <= 3) {
				color[0] = (int)(255 - ((c - 1) * 32));
			}
			else if (c <= 6) {
				color[1] = (int)(255 - ((c - 4) * 32));
			}
			else if (c <= 9) {
				color[2] = (int)(255 - ((c - 7) * 32));
			}
			printf("Key:%d, RGB(%d,%d,%d)\n", c, color[0], color[1], color[2]);
			break;
	
		case 'w': // UP
		case 'W':
			rotationX += rotationSpeed;
			printf("Rot %f,%f\n", rotationX, rotationY);
		case 'a': //LEFT
		case 'A':
			rotationY += rotationSpeed;
			printf("Rot %f,%f\n", rotationX, rotationY);
		case 's': // BOTTOM
		case 'S':
			rotationX -= rotationSpeed;
			printf("Rot %f,%f\n", rotationX, rotationY);
		case 'd': // RIGHT
		case 'D':
			rotationY -= rotationSpeed;
			printf("Rot %f,%f\n", rotationX, rotationY);

		case 'i': // FRONT
		case 'I':
			mvY += rotationSpeed;
			printf("Mov %f,%f\n", rotationX, rotationY);
		case 'j': // LEFT
		case 'J':
			mvX -= rotationSpeed;
			printf("Mov %f,%f\n", rotationX, rotationY);
		case 'k': // BACK
		case 'K':
			mvY -= rotationSpeed;
			printf("Mov %f,%f\n", rotationX, rotationY);
		case 'l': // RIGHT
		case 'L':
			mvX += rotationSpeed;
			printf("Mov %f,%f\n", rotationX, rotationY);

		case ' ':
			// Clear the frame on space
			makeCheckImage(col);
			break;
		case 't':
		case 'T':
			/* Change Texture */
			activeTextureId++;
			if (activeTextureId > textureCount)
				activeTextureId = 0;
			printf("Texture Selected: %d/%d\n", activeTextureId, textureCount);
			break;
		case 'h':
		case 'H':
			/* Instructions */
			help();
			break;
		case 27:
		case 'q':
		case 'Q':
			/* quit the program */
			exit(0);
			break;
		default:
			break;
	}
	// Request diplay update
	glutPostRedisplay();
	return;
}

/* handle mouse interaction */
void mouseInput(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			/* this is called when the button is first pushed down */
			fprintf(stderr, "M-Dn-%d\t%d\n", x, y);
			glutPostRedisplay();
		}
		else {
			/* this is called when the button is released */
			fprintf(stderr, "M-Up-%d\t%d\n", x, y);
			glutPostRedisplay();
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	}
	return;
}

/* On motion of the mouse key down */
void mouseMotion(int x, int y) {
	//fprintf(stderr,"Pt@(%d,\t%d)\n", x, y);
	setPixel(x, y);
	glutPostRedisplay();
}


/* Read a bitmap into the frame buffer */
void readImageToBuffer(CImg<unsigned char> *image, unsigned int ***checkImage) {
	try {
		if (image->height() > window_height || image->width() > window_width) {
			printf("Image size is bigger than the window size");
		}
		int img_height_idx = image->height() - 1;

		//Iterating over image buffer image rows
		for (int row = 0; row < image->height(); row++) {
			unsigned char * ptr_r = image->data(0, row, 0, 0); //red pixels array 1'st row
			unsigned char * ptr_g = image->data(0, row, 0, 1); //green pixels array 1'st row
			unsigned char * ptr_b = image->data(0, row, 0, 2); //blue pixels array 1'st row
			unsigned char * ptr_a = image->data(0, row, 0, 3); //alpha array 1'st row
			for (int col = 0; col < image->width(); col++) {
				// Writing all rgb chanel values Update the buffer 
				checkImage[img_height_idx - row][col][0] = (GLubyte)*(ptr_r);
				checkImage[img_height_idx - row][col][1] = (GLubyte)*(ptr_g);
				checkImage[img_height_idx - row][col][2] = (GLubyte)*(ptr_b);
				ptr_r++;
				ptr_g++;
				ptr_b++;
			}
		}
		printf("\nPixels Read \n");
	}
	catch (CImgIOException&) {
		printf("Unable to read image");
		exit(0);
	}
}

/* Read a bitmap into the frame buffer */
void cimgDisplayBuffer(CImg<unsigned char> *image) {
	try {
		if (image->height() > window_height || image->width() > window_width) {
			printf("Image size is bigger than the window size");
			exit(0);
		}
		int img_height = image->height();
		printf("Reading the image %s %d x %d\n", fileFullPath, img_height, image->width());
		//Iterating over image buffer image rows
		for (int row = 0; row < img_height; row++) {
			unsigned char * ptr_r = image->data(0, row, 0, 0); //red pixels array 1'st row
			unsigned char * ptr_g = image->data(0, row, 0, 1); //green pixels array 1'st row
			unsigned char * ptr_b = image->data(0, row, 0, 2); //blue pixels array 1'st row
			unsigned char * ptr_a = image->data(0, row, 0, 3); //alpha array 1'st row
			for (int col = 0; col < image->width(); col++) {
				// Writing all rgb chanel values Update the buffer 
				checkImage[img_height - row][col][0] = (GLubyte)*(ptr_r);
				checkImage[img_height - row][col][1] = (GLubyte)*(ptr_g);
				checkImage[img_height - row][col][2] = (GLubyte)*(ptr_b);
				ptr_r++;
				ptr_g++;
				ptr_b++;
			}
		}
		printf("\nPixels Read \n");
	}
	catch (CImgIOException&) {
		printf("Unable to read image");
		exit(0);
	}
}

/* Read a texture and setup the context */
GLuint makeTexture(std::string textureString, int tId)
{
	CImg<unsigned char> *imgTex = NULL;
	try {
		textureString = "assets/" + textureString;
		printf("Texture file Name :: %s\n", textureString.c_str());
		imgTex = new CImg<unsigned char>(textureString.c_str());
	}
	catch (CImgIOException&) {
		printf("Unable to read image");
		exit(0);
	}

	int img_height = imgTex->height(), img_width = imgTex->width(), img_depth = 3;
	printf("Reading the image: %d x %d\n", img_height, img_width);
	// Create Image buffer to hold the texture
	unsigned int ***imgPixels = new unsigned int **[img_height]();
	for (int i = 0; i < img_height; i++) {
		imgPixels[i] = new unsigned int *[img_width]();
		for (int j = 0; j < img_width; j++) {
			imgPixels[i][j] = new unsigned int[img_depth]();
		}
	}
	// Read image to the array
	readImageToBuffer(imgTex, imgPixels);
	//cimgDisplayBuffer(imgTex);

	/* Initialize the Texture */
	if (textureIds == NULL) //Initalize the first time
		textureIds = new GLuint[mzParser->texturesList.size()];

	//upload to GPU texture
	glGenTextures(1, &textureIds[tId]);
	glBindTexture(GL_TEXTURE_2D, textureIds[tId]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, window_height, window_width, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, img_height, img_width, 0, GL_RGB, GL_UNSIGNED_INT, imgPixels);

	// Set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deallocate the image buffer
	for (int i = 0; i < img_height; i++) {
		for (int j = 0; j < img_width; j++)
			delete[] imgPixels[i][j];
		delete[] imgPixels[i];
	}
	delete[] imgPixels;
	if(imgTex != NULL)
		delete imgTex;
	printf("Texture id: %d \n", textureIds[tId]);
	return textureIds[tId];
}

/* Read all the textures and set up the context */
void makeTextures(std::vector<std::string> texturesList)
{
	GLubyte col[] = { 0, 0, 0};
	int i = 0;
	for (size_t i = 0; i < texturesList.size(); i++) {
		printf("Setting up texture[%d] for: %s \n", i, texturesList[i].c_str());
		if (i == 0)
			col[2] = 255; // B
		else{
			col[1] = 255; // G
			col[2] = 0;
		}
		makeCheckImage(col);

		makeTexture(texturesList[i], i);
		textureCount++;
	}

}


/* Read the maze scene information from the maze*.txt */
void readMazeScene() {
	// Parse the maze floorplan
	mzParser->readMaze();
	// Update the framebuffer with maze floor plan
	// mzParser->createMazeFramebuffer(checkImage);
	mzParser->printConfigurations();
	//getchar();
}






/*
* Main entry point of the application
*/
int main(int argc, char **argv) {
	if (argc != 2) { 
		/* argc should be 2 for correct execution */
		/* We print argv[0] assuming it is the program name */
		const char * imageFileName = "assets/maze2.txt";
		printf("usage: %s filename", argv[0]);
		sprintf_s(fileFullPath, "%s", imageFileName);
		printf("Default file Name :: %s\n", fileFullPath);
	} else {
		// We assume argv[1] is a filename to open
		sprintf_s(fileFullPath, "%s", argv[1]);
	}
	printf("Custom maze file :: %s\n", fileFullPath);
	mzParser = new MzParser(fileFullPath);

	/* Initialize GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Maze Explorer");

	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	
	// Read the maze file
	readMazeScene();
	printf("Create Textures \n");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	makeTextures(mzParser->texturesList);
	printf("Display Texture ");
	display();
	init();
	printf("End");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	// set an idle function
	glutIdleFunc(redisplay);
	// init the mouse glut callback functions
	glutMouseFunc(mouseInput);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(NULL);
	// Enter main loop
	glutMainLoop();
	free(checkImage);


	return 1;
}