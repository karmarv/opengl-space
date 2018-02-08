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
void Triangle::rasterizeTriangle(Matrix4 mvpMatrix, int id) {

	// Vertex Vectors
	Vector4 vc1(v[0].x, v[0].y, v[0].z, v[0].w);
	Vector4 vc2(v[1].x, v[1].y, v[1].z, v[1].w);
	Vector4 vc3(v[2].x, v[2].y, v[2].z, v[2].w);
	// Projecting the Vertices
	Vector4 vp1 = mvpMatrix * vc1;
	Vector4 vp2 = mvpMatrix * vc2;
	Vector4 vp3 = mvpMatrix * vc3;

	// Homogenization of the Vertices
	Vertex vp[3];
	vp[0].set(vp1.x / vp1.w, vp1.y / vp1.w, vp1.z / vp1.w);
	vp[1].set(vp2.x / vp2.w, vp2.y / vp2.w, vp2.z / vp2.w);
	vp[2].set(vp3.x / vp3.w, vp3.y / vp3.w, vp3.z / vp3.w);

	// If not in canonical form then return (clipping out the material)
	if (!((-1 <= vp[0].x) && (vp[0].x <=1) && (-1 <= vp[0].y) && (vp[0].y <= 1) && 
		(-1 <= vp[1].x) && (vp[1].x <= 1) && (-1 <= vp[1].y) && (vp[1].y <= 1) &&
		(-1 <= vp[2].x) && (vp[2].x <= 1) && (-1 <= vp[2].y) && (vp[2].y <= 1))) {
		return;
	}

	// View port transformation from canonical[-1, 1] to screen space[0, 1000]
	vp[0].x = (vp[0].x + 1) * (window_width / 2);
	vp[0].y = (vp[0].y + 1) * (window_height / 2);
	vp[1].x = (vp[1].x + 1) * (window_width / 2);
	vp[1].y = (vp[1].y + 1) * (window_height / 2);
	vp[2].x = (vp[2].x + 1) * (window_width / 2);
	vp[2].y = (vp[2].y + 1) * (window_height / 2);

	// Find the bounding box 
	float y_min = vp[0].y, y_max = vp[0].y,	x_min = vp[0].x, x_max = vp[0].x;
	y_min = max(min(min(vp[0].y, vp[1].y), vp[2].y), 0);
	x_min = max(min(min(vp[0].x, vp[1].x), vp[2].x), 0);
	x_max = min(max(max(vp[0].x, vp[1].x), vp[2].x), window_width);
	y_max = min(max(max(vp[0].y, vp[1].y), vp[2].y), window_height);

	// Randomize the color based on the id passed
	int color[3] = { 0, 255, 0 };
	if (id % 2 == 0) {
		color[0] = (150 * id) % 255; // rand() %
		color[1] = 0;
	} else {
		color[0] = 0;
		color[1] = (150 * id) % 255;
	}

	// Basic inside triangle test 
	int y = 0, x = 0;
	int inCount = 0;
	for (y = y_min; y <= y_max; y++) {
		for (x = x_min; x <= x_max; x++) {
			if (insideTriangle(x, y, vp)) {			
				fb.setColorBuffer(x, y, color);
				inCount++;
			}
		}
	}
}

/* Line equation */
void Triangle::lineEquation(Vertex *f, Vertex *v1, Vertex *v2) {
	crossProduct(f, v1, v2);
}
/* Dot product between vertices */
float Triangle::dotProduct(Vertex *v1, Vertex *v2) {
	return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}
/* Cross product between vertices */
void Triangle::crossProduct(Vertex *res_vector, Vertex *v1, Vertex *v2) {
	res_vector->x = (v1->y * v2->z) - (v1->z * v2->y); //[0]
	res_vector->y = (v1->z * v2->x) - (v1->x * v2->z); //[1]
	res_vector->z = (v1->x * v2->y) - (v1->y * v2->x); //[2]
}

/* Inside triangle test */
bool Triangle::insideTriangle(int x, int y, Vertex vp[3]) {
	// v0 [ax, ay, az] x v1 [bx, by, bz] = { ay.bz - az.by; az.bx - ax.bz; ax.by - ay.bx }
	// Keeping Z=1 for all vertices/point
	vp[0].z = 1; vp[1].z = 1; vp[2].z = 1;
	Vertex f0, f1, f2;
	lineEquation(&f0, &vp[0], &vp[1]);
	lineEquation(&f1, &vp[1], &vp[2]);
	lineEquation(&f2, &vp[2], &vp[0]);
	Vertex p(x, y, 1);
	if ((dotProduct(&p, &f0) > 0) &&
		(dotProduct(&p, &f1) > 0) &&
		(dotProduct(&p, &f2) > 0)) {
		// Inside the triangle 
		return true;
	}
	return false;
}