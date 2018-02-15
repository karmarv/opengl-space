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

	// If not in canonical form then return (clipping out the triangles on floor)
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
	// Since the Z axis goes from (0.1 to 700) 
	// TODO: Unable to use this as it clips all rect to triangles
	/*float vpz1 = (700 - 0.1) / 2;
	float vpz2 = (700 + 0.1) / 2;
	vp[0].z = vp[0].z * vpz1 + vpz2;
	vp[1].z = vp[1].z * vpz1 + vpz2;
	vp[2].z = vp[2].z * vpz1 + vpz2;
	*/
	// Find the bounding box 
	float y_min = vp[0].y, y_max = vp[0].y,	x_min = vp[0].x, x_max = vp[0].x;
	y_min = max(min(min(vp[0].y, vp[1].y), vp[2].y), 0);
	x_min = max(min(min(vp[0].x, vp[1].x), vp[2].x), 0);
	x_max = min(max(max(vp[0].x, vp[1].x), vp[2].x), window_width);
	y_max = min(max(max(vp[0].y, vp[1].y), vp[2].y), window_height);

	// Interpolate the colors
	float color[3] = { 0, 255, 0 };
	/*
		Setup the Vertices of the triangles Texture 
	*/
	Vertex vtx[3];
	vtx[0].set(coords[0][0], coords[0][1], 1);
	vtx[1].set(coords[1][0], coords[1][1], 1);
	vtx[2].set(coords[2][0], coords[2][1], 1);
	float A = getArea(vp[0], vp[1], vp[2]);
	float Ai = 1 / A;

	// Basic inside triangle test 
	Vertex p(0, 0, 1);	// Point inside triangle
	int y = 0, x = 0;
	int inCount = 0;
	for (y = y_min; y <= y_max; y++) {
		for (x = x_min; x <= x_max; x++) {
			p.x = x;
			p.y = y;
			if (insideTriangle(p, vp)) {
				//Interpolate color
				Vertex w(vp1.w, vp2.w, vp3.w);
				// Interpolate the coordinates using Barycentric 
				if(tex){
	
					// Per pixel Color interpolation based on Barycentric coordinates 
					Vector3 pxy(0, 0, 0);
					/* A0, A1, A2: three triangles formed around point P(x,y) */
					float A0 = getArea(p, vp[1], vp[2]);
					float A1 = getArea(p, vp[2], vp[0]);
					float A2 = getArea(p, vp[0], vp[1]);
					float wp = (A0 *Ai) * (1.0 / vp1.w) + (A1 *Ai) * (1.0 / vp2.w) + (A2 * Ai) * (1.0 / vp3.w);
					
					// Perspective correct texture interpolation
					pxy.x = tex->width  * ((A0 * Ai) * (vtx[0].x / vp1.w) + (A1 * Ai) * (vtx[1].x / vp2.w) + (A2 * Ai) * (vtx[2].x / vp3.w)); // Px
					pxy.y = tex->height * ((A0 * Ai) * (vtx[0].y / vp1.w) + (A1 * Ai) * (vtx[1].y / vp2.w) + (A2 * Ai) * (vtx[2].y / vp3.w)); // Py
					pxy.z = (A0 *Ai) * (vp1.z / vp1.w) + (A1 *Ai) * (vp2.z / vp2.w) + (A2 * Ai) * (vp3.z / vp3.w);
					pxy.x /= wp;
					pxy.y /= wp;
					pxy.z /= wp;

					// Map the texture 
					tex->getFloatColor(pxy.x, pxy.y, color); 
					fb.setColorBuffer(x, y, pxy.z, color);
				}else{
					Vector3 col = getColorInterpolation(p, vp, Ai, w);
					color[0] = col.x;
					color[1] = col.y;
					color[2] = col.z;
					fb.setColorBuffer(x, y,1, color);
				}
				inCount++;
			}
		}
	}
}

Vector3 Triangle::getTexInterpolation(Vertex p, Vertex vtx[3], float Ati, Vertex vp[3], float Ai, Vertex w) {


	Vector3 pxy(0, 0, 0);

 	return pxy;
}

Vector3 Triangle::getColorInterpolation(Vertex p, Vertex vp[3], float Ai, Vertex w) {
	Vector3 color(0,0,0);
	float A0 = getArea(p, vp[1], vp[2]);
	float A1 = getArea(p, vp[2], vp[0]);
	float A2 = getArea(p, vp[0], vp[1]);
	// Per pixel Color interpolation based on Barycentric coordinates 
	// Perspective correct interpolation
	color[0] = ((A0 * Ai) * (c[0][0] / w.x) + (A1 * Ai) * (c[1][0] / w.y) + (A2 * Ai) * (c[2][0] / w.z)) * 255; // R
	color[1] = ((A0 * Ai) * (c[0][1] / w.x) + (A1 * Ai) * (c[1][1] / w.y) + (A2 * Ai) * (c[2][1] / w.z)) * 255; // G
	color[2] = ((A0 * Ai) * (c[0][2] / w.x) + (A1 * Ai) * (c[1][2] / w.y) + (A2 * Ai) * (c[2][2] / w.z)) * 255; // B
	float wp = (A0 *Ai) * (1.0 / w.x) + (A1 *Ai) * (1.0 / w.y) + (A2 * Ai) * (1.0 / w.z);
	color[0] /= wp;
	color[1] /= wp;
	color[2] /= wp;
	return color;
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
bool Triangle::insideTriangle(Vertex p, Vertex vp[3]) {
	// v0 [ax, ay, az] x v1 [bx, by, bz] = { ay.bz - az.by; az.bx - ax.bz; ax.by - ay.bx }
	// Keeping Z=1 for all vertices/point
	float z1 = vp[0].z, z2 = vp[1].z, z3 = vp[2].z;
	vp[0].z = 1; vp[1].z = 1; vp[2].z = 1;
	Vertex f0, f1, f2;
	lineEquation(&f0, &vp[0], &vp[1]);
	lineEquation(&f1, &vp[1], &vp[2]);
	lineEquation(&f2, &vp[2], &vp[0]);
	vp[0].z = z1;
	vp[1].z = z2;
	vp[2].z = z3;
	if ((dotProduct(&p, &f0) * dotProduct(&f0, &vp[2]) > 0) &&
		(dotProduct(&p, &f1) * dotProduct(&f1, &vp[0]) > 0) &&
		(dotProduct(&p, &f2) * dotProduct(&f2, &vp[1]) > 0)) {
		// Inside the triangle 
		return true;
	}
	return false;
}

/* Fetch the area of this triangle  */
float Triangle::getArea(Vertex v0, Vertex v1, Vertex v2) {
	// Obtain the area by cross product
	double dArea = ((v1.x - v0.x)*(v2.y - v0.y) - (v2.x - v0.x)*(v1.y - v0.y)) / 2.0;
	return (float)(dArea > 0.0) ? dArea : -dArea;
}