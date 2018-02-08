#include "Scene.h"

extern FrameBuffer fb;
/* Matrix */
Matrix4 M;
Matrix4 P;

float dot(Vertex v1, float *v2) {
	return ((v1.x * v2[0]) + (v1.y * v2[1]) + (v1.z * v2[2]));
}

Matrix4 setFrustum(float l, float r, float b, float t, float n, float f)
{
	Matrix4 mat;
	mat[0] = 2 * n / (r - l);
	mat[5] = 2 * n / (t - b);
	mat[8] = (r + l) / (r - l);
	mat[9] = (t + b) / (t - b);
	mat[10] = -(f + n) / (f - n);
	mat[11] = -1;
	mat[14] = -(2 * f * n) / (f - n);
	mat[15] = 0;
	return mat;
}

Matrix4 setFrustum(float fovY, float aspect, float front, float back)
{
	const float DEG2RAD = 3.141593f / 180.0f;
	float tangent = tanf(fovY / 2 * DEG2RAD); // tangent of half fovY
	float height = front * tangent;         // half height of near plane
	float width = height * aspect;          // half width of near plane
											// params: left, right, bottom, top, near, far
	return setFrustum(-width, width, -height, height, front, back);
}

//TODO: REMOVE All OpenGL references
/* set the perspective projection matrix given the following values */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float front, float back) {
	/* TODO: Test */
	P.identity();
	P = setFrustum(eye_fov, aspect_ratio, front, back);
	//printf("Set Projection\n");
}


//TODO: REMOVE All OpenGL references
/* set the modelview matrix with the given parameters */
void setModelviewMatrix(float *eye_pos, float eye_theta, float eye_phi) {
	/* Matrix
		v′ = (T(R(Sv))) This transformation can be stated in words as “first scale, then rotate, then translate”.
	*/
	M.identity();
	/* translate to get the eye origin in the center of the coordinate system */
	M.translate(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
	/* now rotate by theta */
	M.rotate(-eye_theta, 0, 0, 1);
	/* now rotate by phi */
	M.rotate(-eye_phi, 1, 0, 0);
}


/* this implements the software rendering of the scene */ 
void Scene :: renderSceneSoftware(void) {
	/* this is the function you will write.  you will need to take the linked list of triangles
	given by *original_head and draw them to the framebuffer fb */
	
	/* now clear all the stuff */
	glClearColor(100 / (float)255, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	fb.clear();
	
	// Test triangles (Order is critical, otherwise the inside triangle logic doesnt work properly.)
	/*
	1. The vertex coordinate should be in homogeneous coordinates, if you are going to multiply it with the model-view and projection matrices:
		Vertex v1 = { 0.0f,0.0f,1.0f,1.0f };
		But if that is your screen coordinate, it is fine.
	*/
	// Lets draw a fixed test triangle 
	Vertex v1_test = { 0.0f,0.0f,1.0f,1.0f };
	Vertex v2_test = { 1.0f,1.0f,1.0f,1.0f };
	Vertex v3_test = { 0.0f,1.0f,1.0f,1.0f };

	/* Final View Matrix */
	Matrix4 mvpMatrix = P * M;

	Triangle test(&v1_test, &v2_test, &v3_test);
	test.mvpVertices(mvpMatrix);
	test.rasterizeTriangle();

	int count = 0;
	TriangleList *ptr;
	/* now render the triangles in the list 
	for (ptr = original_head; ptr;) {
		count++;
		ptr->t->mvpVertices(mvpMatrix);
		ptr->t->rasterizeTriangle();
		ptr = ptr->next;
		if (count > 5)
			break;
	}*/
	
	printf("%d.",count);
	glDisable(GL_DEPTH_TEST);
	return;
}