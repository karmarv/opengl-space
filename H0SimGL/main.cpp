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

#include <gl/glut.h>	//  Include GLUT, OpenGL, and GLU libraries
#include "CImg.h"		//  CImg C Library

using namespace cimg_library;

/************************** VARIABLES ********************************/
#define window_width	512
#define window_height	512

char imageFullPath[255];
CImg<unsigned char> *image = NULL;
GLubyte checkImage[window_height][window_width][3]; // Static allocation of frame buffer
//GLubyte ***checkImage = NULL;
GLubyte color[] = { 0, 0, 0, 255 };
int brush_size = 1;


/************************** FUNCTIONS ********************************/

/* Initialize the framebuffer with white color */
void initFrameBufferPixels(void)
{
	brush_size = 1;
	int i, j, c;
	for (i = 0; i < window_height; i++) {
		for (j = 0; j < window_width; j++) {
			//Color the frame buffer white
			c = 255;// ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			//fprintf(stderr, "%d\t%d\n", i, j);
		}
	}
}

/* Read a bitmap into the frame buffer */
void cimgDisplayBuffer() {
	try {
		if (image->height() > window_height || image->width() > window_width) {
			printf("Image size is bigger than the window size");
			exit(0);
		}
		int img_height = image->height();
		printf("Reading the image %s %d x %d\n", imageFullPath, img_height, image->width());
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

/* Dump the frame buffer into a bitmap */
void cimgSaveBuffer() {
	try{
		printf("Saving the image from frame buffer.\n");
		CImg<unsigned char> img(window_height, window_width, 1, 3);
		int img_height = img.height();
		//Iterating over image buffer image rows
		for (int row = 0; row < img_height; row++) {
			for (int col = 0; col < img.width(); col++) {
				// Writing all rgb chanel values, Update the buffer 
				img(col, row, 0) = checkImage[img_height - row][col][0];
				img(col, row, 1) = checkImage[img_height - row][col][1]; // different colour on a pixel - green
				img(col, row, 2) = checkImage[img_height - row][col][2];
			}
		}
		img.save_bmp("artsy_out.bmp"); // save it
	}
	catch (CImgIOException&) {
		printf("Unable to read image");
		exit(0);
	}
}

/* shape the view port upon redisplay */
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* Set the color on a particular pixel in frame buffer */
void setPixel(int x, int y) {
	
	int x1=x- brush_size, 
		y1=window_height-(y- brush_size), 
		x2= x+ brush_size, 
		y2= window_height - (y+ brush_size);
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
		fprintf(stderr,"%s: found gl error: %s\n",prefix, gluErrorString(error));
		exit(-1);
	}
}

/* Opengl draw the framebuffer on the display */
void display(void) {
	glutSetWindowTitle("SimpleDraw");
	//  variable representing the window title
	//const char *error_msg = "GLUT Window Template";
	//checkGLErrors("Errors in display()!\n");
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(window_width, window_height, GL_RGB,
		GL_UNSIGNED_BYTE, checkImage);
	glFlush();

	glutSwapBuffers();
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
	printf("\t 4. Save \t\t: s/S \n");
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
			}else if (c <= 3) {
				color[0] = (int)(255 - ((c - 1) * 32));
			}else if (c <= 6) {
				color[1] = (int)(255 - ((c - 4) * 32));
			}else if (c <= 9) {
				color[2] = (int)(255 - ((c - 7) * 32));
			}
			printf("Key:%d, RGB(%d,%d,%d)\n", c,color[0], color[1], color[2]);
			break;
		case ' ':
			// Clear the frame on space
			initFrameBufferPixels();
			glutPostRedisplay();
			break;
		case 'b':
		case 'B':
			/* Increase pixel size */
			brush_size++;
			if (brush_size > 20)
				brush_size = 1;
			break;
		case 'h':
		case 'H':
			/* Instructions */
			help();
			break;
		case 'i':
		case 'I':
			/* Show the background image */
			cimgDisplayBuffer();
			glutPostRedisplay();
			break;
		case 's':
		case 'S':
			/* Save the image */
			cimgSaveBuffer();
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

	return;
}


/* handle mouse interaction */
void mouseInput(int button, int state, int x, int y) {
    switch(button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) {
				/* this is called when the button is first pushed down */
				fprintf(stderr,"M-Dn-%d\t%d\n", x, y);
				glutPostRedisplay();
			}
			else {
				/* this is called when the button is released */
				fprintf(stderr,"M-Up-%d\t%d\n", x, y);
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



/* Initialize the state and buffers */
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	help();
	cimgDisplayBuffer();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

/* Dynamic Image buffer allocation 
void allocateFrameBuffer(int h, int w, int c) {
	printf("Buffer size: %dx%dx%d \n", h,w,c);
	checkImage = (GLubyte ***)malloc(h * sizeof(GLubyte **));
	for (int i = 0; i < h; i++) {
		checkImage[i] = (GLubyte **)malloc(w * sizeof(GLubyte *));
		for (int j = 0; j < w; j++) {
			checkImage[i][j] = (GLubyte *)malloc(c * sizeof(GLubyte));
		}
	}
}*/

/* 
 * Main entry point of the application 
 */
int main(int argc, char **argv) {
	if (argc != 2){ /* argc should be 2 for correct execution */
		/* We print argv[0] assuming it is the program name */
		const char * imageFileName = "lena_color.bmp";
		printf("usage: %s filename", argv[0]);
		sprintf_s(imageFullPath, "%s", imageFileName);
		printf("Default file Name :: %s\n", imageFullPath);
	} else {
		// We assume argv[1] is a filename to open
		sprintf_s(imageFullPath, "%s", argv[1]);
	}
	try {
		printf("Custom file Name :: %s\n", imageFullPath);
		image = new CImg<unsigned char>(imageFullPath);
	}
	catch (CImgIOException&) {
		printf("Unable to read image");
		exit(0);
	}
	
	//int h = image->height(), w = image->width();
	//Allocate frame buffer 
	//allocateFrameBuffer(h+1, w+1, 3);
	//printf("Initialized Image: %d x %d = %d\n", h, w, checkImage[511][511][2]);

    /* Initialize GLUT */
    glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize(window_width, window_height);
	glutCreateWindow("window_name");
    glutInitWindowPosition(400,50);

	init();
	
	glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
	
	/* set an idle function */
	glutIdleFunc(redisplay);

	/* init the mouse glut callback functions */
    glutMouseFunc(mouseInput);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(NULL);

    /* Enter main loop */
	glutMainLoop();
	free(checkImage);

	return 1;
}