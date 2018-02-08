#include "Triangle.h"

extern FrameBuffer fb;

/* for openGL rendering of the triangle */
void Triangle :: renderOpenGL(void) {
	/* first bind the texture */
	if (tex) {
		tex->bind();
	}

	/* now draw the triangle */
	glBegin(GL_TRIANGLES);
		glTexCoord2fv(coords[0]);
		glColor3fv(c[0]);
		glVertex3f(v[0].x, v[0].y, v[0].z);

		glTexCoord2fv(coords[1]);
		glColor3fv(c[1]);
		glVertex3f(v[1].x, v[1].y, v[1].z);

		glTexCoord2fv(coords[2]);
		glColor3fv(c[2]);
		glVertex3f(v[2].x, v[2].y, v[2].z);
	glEnd();

	/* release the texture */
	if (tex) {
		tex->release();
	}

	return;
}

/* Draw triangles in the frame buffer */
void Triangle::rasterizeTriangle(void) {
	// Find the bounding box 
	float y_min = v[0].y, y_max = v[0].y,
		  x_min = v[0].x, x_max = v[0].x;
	// Find the bounds of the triangle
	for (Vertex v : v) {
		if (v.y < y_min)
			y_min = v.y;
		if (v.y >= y_max)
			y_max = v.y;
		if (v.x < x_min)
			x_min = v.x;
		if (v.x >= x_max)
			x_max = v.x;
	}

	// View port transformation from canonical[-1, 1] to screen space[0, 1000]
	v[0].x = (v[0].x + 1) * (window_width / 2);
	v[0].y = (v[0].y + 1) * (window_height / 2);
	v[1].x = (v[1].x + 1) * (window_width / 2);
	v[1].y = (v[1].y + 1) * (window_height / 2);
	v[2].x = (v[2].x + 1) * (window_width / 2);
	v[2].y = (v[2].y + 1) * (window_height / 2);

	//printf("V1(%f,%f), V2(%f,%f) \n", x_min, y_min, x_max, y_max);
	x_min = (x_min + 1) * (window_width / 2);
	y_min = (y_min + 1) * (window_width / 2);
	x_max = (x_max + 1) * (window_width / 2);
	y_max = (y_max + 1) * (window_width / 2);

	// Make sure the points lie in the screen coordinates
	if (y_max > window_height)
		y_max = window_height;
	if (x_max > window_width)
		x_max = window_width;
	if (y_min < 0)
		y_min = 0;
	if (x_min < 0)
		x_min = 0;

	// Inside triangle test 
	int color[3] = { 0, 255, 0 };
	int y = 0, x = 0;
	int inCount = 0;
	for (y = y_min; y <= y_max; y++) {
		for (x = x_min; x <= x_max; x++) {
			if (insideTriangle(x, y)) {
				//printf("In (%d,%d)\n", x, y);
				fb.setColorBuffer(x, y, color);
				inCount++;
			}
		}
	}
	//printf("In Vertices: %d\n", inCount);
	//print();
}

void Triangle::lineEquation(Vertex *f, Vertex *v1, Vertex *v2) {
	crossProduct(f, v1, v2);
}

float Triangle::dotProduct(Vertex *v1, Vertex *v2) {
	return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

void Triangle::crossProduct(Vertex *res_vector, Vertex *v1, Vertex *v2) {
	res_vector->x = (v1->y * v2->z) - (v1->z * v2->y); //[0]
	res_vector->y = (v1->z * v2->x) - (v1->x * v2->z); //[1]
	res_vector->z = (v1->x * v2->y) - (v1->y * v2->x); //[2]
}

/* Inside triangle test */
bool Triangle::insideTriangle(int x, int y) {
	// v0 [ax, ay, az] x v1 [bx, by, bz] = { ay.bz - az.by; az.bx - ax.bz; ax.by - ay.bx }
	// TODO: Fix, Keeping Z=1 for all vertices/point
	v[0].z = 1; v[1].z = 1; v[2].z = 1;
	Vertex f0, f1, f2;
	lineEquation(&f0, &v[0], &v[1]);
	lineEquation(&f1, &v[1], &v[2]);
	lineEquation(&f2, &v[2], &v[0]);
	Vertex p(x, y, 1);
	if ((dotProduct(&p, &f0) > 0) &&
		(dotProduct(&p, &f1) > 0) &&
		(dotProduct(&p, &f2) > 0) ) {
		// Inside the triangle 
		return true;
	}
	return false;
}


/* Model View Projection Matrix */
void Triangle::mvpVertices(Matrix4 mvpMatrix)
{
	// Vertex Vectors
	Vector4 vc1(v[0].x, v[0].y, v[0].z, v[0].w);
	Vector4 vc2(v[1].x, v[1].y, v[1].z, v[1].w);
	Vector4 vc3(v[2].x, v[2].y, v[2].z, v[2].w);
	// Projecting the Vertices
	Vector4 vp1 = mvpMatrix * vc1;
	Vector4 vp2 = mvpMatrix * vc2;
	Vector4 vp3 = mvpMatrix * vc3;
	// Homogenization of the Vertices
	v[0].set(vp1.x / vp1.w, vp1.y / vp1.w, vp1.z / vp1.w);
	v[1].set(vp2.x / vp2.w, vp2.y / vp2.w, vp2.z / vp2.w);
	v[2].set(vp3.x / vp3.w, vp3.y / vp3.w, vp3.z / vp3.w);
}
