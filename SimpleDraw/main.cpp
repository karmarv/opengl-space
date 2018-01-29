
#include <stdio.h>
#include <windows.h>

/* include gl and glut files */
#include "gl.h"
#include "glu.h"
#include "glut.h"

#include "globals.h"


/**********************************************************/
#define checkImageWidth window_width
#define checkImageHeight window_height
GLubyte checkImage[checkImageHeight][checkImageWidth][3];
GLubyte color[] = { 0, 0, 0, 255 };

void initFrameBufferPixels(void)
{
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			//Color the frame buffer white
			c = 255;// ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			//fprintf(stderr, "%d\t%d\n", i, j);
		}
	}
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	initFrameBufferPixels();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void setPixel(int x, int y) {
	int size = 1;
	int x1=x-size, y1=window_height-(y-size), x2= x+size, y2= window_height - (y+size);
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

void display(void) {
	glutSetWindowTitle("SimpleDraw");

	checkGLErrors("Errors in display()!\n");
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB,
		GL_UNSIGNED_BYTE, checkImage);
	glFlush();

	glutSwapBuffers();
}

void redisplay(void) {
    glutPostRedisplay();
	return;
}

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

void mouseMotion(int x, int y) {
	fprintf(stderr,"Pt@(%d,\t%d)\n", x, y);
	setPixel(x, y);
    glutPostRedisplay();
}


int main(int argc, char **argv) {

	//cimgDisplay();
	
    /* Initialize GLUT */
    glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize(window_width, window_height);
	glutCreateWindow("window_name");
    glutInitWindowPosition(100,50);

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

	return 1;
}