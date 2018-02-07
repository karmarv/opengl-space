#include "Scene.h"

extern FrameBuffer fb;


//TODO: REMOVE All OpenGL references
/* set the perspective projection matrix given the following values */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float near, float far) {
	/* TODO: remove openGL mode */

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


}

/* this implements the software rendering of the scene */ 
void Scene :: renderSceneSoftware(void) {
	/* this is the function you will write.  you will need to take the linked list of triangles
	given by *original_head and draw them to the framebuffer fb */
	
    /* now clear all the stuff */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Add test pattern in framebuffer
	fb.makeCheckImage();

	// Test triangles (Order is critical, otherwise the inside triangle logic doesnt work properly.)
	/*
	1. The vertex coordinate should be in homogeneous coordinates, if you are going to multiply it with the model-view and projection matrices:
		Vertex v1 = { 0.0f,0.0f,1.0f,1.0f };
		But if that is your screen coordinate, it is fine.
	*/
	Vertex v1 = { 0.0f,0.0f,1.0f,0.0f };
	Vertex v2 = { 500.0f,500.0f,1.0f,0.0f };
	Vertex v3 = { 0.0f,500.0f,1.0f,0.0f };
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


	
	return;
}