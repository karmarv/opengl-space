#include "Scene.h"

extern FrameBuffer fb;


//TODO: REMOVE All OpenGL references
/* set the perspective projection matrix given the following values */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float near, float far) {
	/* TODO: remove openGL mode */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(eye_fov,		/* field of view in degree */
		(double)window_width / (double)window_height, /* aspect ratio */
		10,		/* near plane */
		700);		/* far plane */
}

//TODO: REMOVE All OpenGL references
/* set the modelview matrix with the given parameters */
void setModelviewMatrix(float *eye_pos, float eye_theta, float eye_phi) {

	/// Matrix 
	Matrix4 m2(1, 1, 1, 1,   // 1st column
				1, 0, 1, 1,   // 2nd column
				1, 1, 1, 1,   // 3rd column
				1, 0, 1, 0);

	m2.identity();

	/* TODO: remove openGL mode */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* now rotate by phi */
	glRotatef(-eye_phi, 1, 0, 0);
	/* now rotate by theta */
	glRotatef(-eye_theta, 0, 0, 1);
	/* translate to get the eye origin in the center of the coordinate system */
	glTranslatef(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
}

/* this implements the software rendering of the scene */ 
void Scene :: renderSceneSoftware(void) {
	/* this is the function you will write.  you will need to take the linked list of triangles
	given by *original_head and draw them to the framebuffer fb */
	
   /* now clear all the stuff */
	glClearColor(100 / (float)255, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Test triangles 
	Vertex v1 = { 15.0f,15.0f,-10.0f,0.0f};
	Vertex v2 = { 10.0f,10.0f,-10.0f,0.0f };
	Vertex v3 = { 10.0f,20.0f,-10.0f,0.0f };
	Triangle test(&v1, &v2, &v3);
	test.rasterizeTriangle();

	TriangleList *ptr;
	/* now render the triangles in the list */
	//for (ptr = original_head; ptr;) {
		//ptr->t->renderOpenGL();
		//ptr = ptr->next;
	//}

	//ptr = original_head;
	//ptr->t->rasterizeTriangle();

	//Add test pattern in framebuffer
	//fb.makeCheckImage();
	
	return;
}