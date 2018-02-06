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
	for (Vertex v : v) {
		//printf("V (%f,%f) \n", v.x, v.y);
		if (v.y < y_min)
			y_min = v.y;
		if (v.y >= y_max)
			y_max = v.y;
		if (v.x < x_min)
			x_min = v.x;
		if (v.x >= x_max)
			x_max = v.x;
	}
	//printf("V1(%f,%f), V2(%f,%f) \n", x_min, y_min, x_max, y_max);
	/*x_min = 0.0f;
	y_min = 0.0f;
	x_max = window_width;
	y_max = window_height;*/
	// Inside triangle test 
	int y = 0, x = 0;
	for (y = y_min; y <= y_max; y++) {
		for (x = x_min; x <= x_max; x++) {
			if (insideTriangle(x, y)) {
				printf("In (%d,%d)", x, y);
			}
		}
	}
	printf(" Vertices: \n");
	print();
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

bool Triangle::insideTriangle(int x, int y) {
	// v0 [ax, ay, az] x v1 [bx, by, bz] = { ay.bz - az.by; az.bx - ax.bz; ax.by - ay.bx }
	// TODO: Fix, Keeping Z=1 for all vertices/point
	v[0].z = 1;
	v[1].z = 1;
	v[2].z = 1;
	Vertex f0, f1, f2;
	lineEquation(&f0, &v[0], &v[1]);
	lineEquation(&f1, &v[1], &v[2]);
	lineEquation(&f2, &v[2], &v[0]);

	Vertex p(x, y, 1);
	if ((dotProduct(&p, &f0) > 0) &&
		(dotProduct(&p, &f1) > 0) &&
		(dotProduct(&p, &f2) > 0) ) {
		// inside the triangle 
		return true;
	}

	return false;
}
