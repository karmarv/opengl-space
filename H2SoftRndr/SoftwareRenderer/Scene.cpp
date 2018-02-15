#include "Scene.h"

extern FrameBuffer fb;

/* ModelView and Projection matrix variable*/
Matrix4 M;
Matrix4 P;

/* Compute dot product of vertices */
float dot(Vertex v1, float *v2) {
	return ((v1.x * v2[0]) + (v1.y * v2[1]) + (v1.z * v2[2]));
}

/* Compute the frustum matrix */
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
/* Sets the frustum of the view */
Matrix4 setFrustum(float fovY, float aspect, float front, float back)
{
	const float DEG2RAD = 3.141593f / 180.0f;
	float tangent = tanf(fovY / 2 * DEG2RAD); // tangent of half fovY
	float height = front * tangent;         // half height of near plane
	float width = height * aspect;          // half width of near plane
											// params: left, right, bottom, top, near, far
	return setFrustum(-width, width, -height, height, front, back);
}

/* set the perspective projection matrix given the following values */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float front, float back) {
	P.identity();
	P = setFrustum(eye_fov, aspect_ratio, front, back);
}


/* set the modelview matrix with the given parameters */
void setModelviewMatrix(float *eye_pos, float eye_theta, float eye_phi) {
	/* Matrix v′ = (T(R(Sv))) This transformation can be stated in words,
		as “first scale, then rotate, then translate”.
	*/
	M.identity();
	/* translate to get the eye origin in the center of the coordinate system */
	M.translate(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
	/* now rotate by theta */
	M.rotate(-eye_theta, 0, 0, 1);
	/* now rotate by phi */
	M.rotate(-eye_phi, 1, 0, 0);
}

/*	NOTE: Test Code
	1. Test triangles (Order is critical, otherwise the inside triangle logic doesnt work properly.)
	2. The vertex coordinate should be in homogeneous coordinates for MVP computation.
*/
void Scene::testRenderSceneSoftware(void){
	/* Final View Matrix */
	Matrix4 mvpMatrix = P * M;

	Texture ** tx = (Texture **)malloc(sizeof(Texture*) * 4);

	tx[0] = new Texture("wall1.tif");
	tx[1] = new Texture("wall4.tif");
	tx[2] = new Texture("wall6.tif");
	tx[3] = new Texture("monaLisa.tif");

	// Lets draw a fixed test triangle 
	Vertex v1_t = { -80.0f,80.0f,0.0f,1.0f };
	Vertex v2_t = { -70.0f,70.0f,0.0f,1.0f };
	Vertex v3_t = { -70.0f,60.0f,20.0f,1.0f };
	Triangle test(&v1_t, &v2_t, &v3_t);
	test.setCoords(0, 0, 0);
	test.setCoords(1, 1, 1);
	test.setCoords(2, 0, 1);
	test.setTexture(tx[0]);
	test.setColor(0, 127, 0, 0); // color[0] = (150 * id) % 255; // rand() %
	test.setColor(1, 0, 255, 0);
	test.setColor(2, 127, 0 , 0);
	test.rasterizeTriangle(mvpMatrix, 1); //G / R

	v1_t = { -60.0f,70.0f,20.0f,1.0f };
	v2_t = { -65.0f,60.0f,20.0f,1.0f };
	v3_t = { -65.0f,50.0f,0.0f,1.0f };
	Triangle test0(&v1_t, &v2_t, &v3_t);
	test0.setCoords(0, 1, 1);
	test0.setCoords(1, 0, 1);
	test0.setCoords(2, 0, 0);
	test0.setTexture(tx[1]);
	test0.setColor(0, 0, 127, 0 );
	test0.setColor(1, 0, 0, 255 );
	test0.setColor(2, 0, 127, 0 );
	test0.rasterizeTriangle(mvpMatrix, 2); //B / G


	v1_t = { -70.0f,80.0f,10.0f,1.0f };
	v2_t = { -80.0f,80.0f,10.0f,1.0f };
	v3_t = { -80.0f,80.0f,0.0f,1.0f };
	Triangle test1(&v1_t, &v2_t, &v3_t);
	test1.setCoords(0, 0, 0);
	test1.setCoords(1, 1, 1);
	test1.setCoords(2, 0, 1);
	test1.setTexture(tx[2]);
	test1.setColor(0, 0, 0, 127);
	test1.setColor(1, 255, 0, 0);
	test1.setColor(2, 0 , 0, 127);
	test1.rasterizeTriangle(mvpMatrix, 3); //R / B

	v1_t = { 0.0f,0.0f,1.0f,1.0f };
	v2_t = { 1.0f,1.0f,1.0f,1.0f };
	v3_t = { 0.0f,1.0f,1.0f,1.0f };
	Triangle test3(&v1_t, &v2_t, &v3_t);
	test3.setCoords(0, 0, 0);
	test3.setCoords(1, 1, 1);
	test3.setCoords(2, 0, 1);
	test3.setTexture(tx[3]);
	test3.setColor(0, 255, 0, 0);
	test3.setColor(1, 0, 255, 0);
	test3.setColor(2, 0, 0, 255);
	test3.rasterizeTriangle(mvpMatrix, 4); // Mixed

	v1_t = { -10.0f,80.0f,60.0f,1.0f };
	v2_t = {  50.0f,80.0f,60.0f,1.0f };
	v3_t = {  50.0f,80.0f,0.0f,1.0f };
	Triangle test4(&v1_t, &v2_t, &v3_t);
	test4.setCoords(0, 1, 1);
	test4.setCoords(1, 0, 1);
	test4.setCoords(2, 0, 0);
	test4.setTexture(tx[3]);
	test4.setColor(0, 0, 127, 0);
	test4.setColor(1, 0, 0, 255);
	test4.setColor(2, 0, 127, 0);
	test4.rasterizeTriangle(mvpMatrix, 5); //Mona

	v1_t = { -10.0f,80.0f,60.0f,1.0f };
	v2_t = {  50.0f,80.0f,0.0f,1.0f };
	v3_t = { -10.0f,80.0f,0.0f,1.0f };
	Triangle test5(&v1_t, &v2_t, &v3_t);
	test5.setCoords(0, 0, 0);
	test5.setCoords(1, 1, 1);
	test5.setCoords(2, 0, 1);
	test5.setTexture(tx[3]);
	test5.setColor(0, 0, 0, 127);
	test5.setColor(1, 255, 0, 0);
	test5.setColor(2, 0, 0, 127);
	test5.rasterizeTriangle(mvpMatrix, 6); //Mona

}

/* this implements the software rendering of the scene */
void Scene :: renderSceneSoftware(void) {
	/* this is the function you will write.  you will need to take the linked list of triangles
	given by *original_head and draw them to the framebuffer fb */
	
	/* now clear all the stuff */
	glClearColor(100 / (float)255, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fb.clear();
	
	/* Final View Matrix */
	Matrix4 mvpMatrix = P * M;

#ifdef DEBUG

	/*Test with static triangles */
	testRenderSceneSoftware();

#endif // DEBUG

#ifndef DEBUG
	int count = 0;
	TriangleList *ptr;
	/* now render the triangles in the list */
	for (ptr = original_head; ptr;) {
		count++;
		ptr->t->rasterizeTriangle(mvpMatrix, count);
		ptr = ptr->next;
		//if (count > 45)
		//	break;
	}
#endif // !DEBUG

	
	return;
}