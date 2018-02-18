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

/* Get projected screen coordinates from world coordinates */
int Triangle::getScreenCoordinates(Matrix4 mvpMatrix, Vertex *vsc) {
	// Vertex Vectors
	Vector4 vc1(v[0].x, v[0].y, v[0].z, v[0].w);
	Vector4 vc2(v[1].x, v[1].y, v[1].z, v[1].w);
	Vector4 vc3(v[2].x, v[2].y, v[2].z, v[2].w);
	// Projecting the Vertices
	Vector4 vp1 = mvpMatrix * vc1;
	Vector4 vp2 = mvpMatrix * vc2;
	Vector4 vp3 = mvpMatrix * vc3;

	/* Normalized Device Coordinates (NDC) */
	vsc[3].set(1.0 / vp1.w, 1.0 / vp2.w, 1.0 / vp3.w); // Keeping the precomputed 1/W perspective divide values
	vsc[0].set(vp1.x * vsc[3].x, vp1.y * vsc[3].x, vp1.z * vsc[3].x);
	vsc[1].set(vp2.x * vsc[3].y, vp2.y * vsc[3].y, vp2.z * vsc[3].y);
	vsc[2].set(vp3.x * vsc[3].z, vp3.y * vsc[3].z, vp3.z * vsc[3].z);

	// If not in canonical form then return (clipping out the triangles on floor)
	if (!((-1 <= vsc[0].x) && (vsc[0].x <= 1) && (-1 <= vsc[0].y) && (vsc[0].y <= 1) &&
		(-1 <= vsc[1].x) && (vsc[1].x <= 1) && (-1 <= vsc[1].y) && (vsc[1].y <= 1) &&
		(-1 <= vsc[2].x) && (vsc[2].x <= 1) && (-1 <= vsc[2].y) && (vsc[2].y <= 1))) {
		return 0;
	}

	// View port transformation from canonical[-1, 1] to screen space[0, 1000]
	vsc[0].x = vp1.x + (vsc[0].x + 1) * (window_width / 2);
	vsc[0].y = vp1.y + (vsc[0].y + 1) * (window_height / 2);
	vsc[1].x = vp2.x + (vsc[1].x + 1) * (window_width / 2);
	vsc[1].y = vp2.y + (vsc[1].y + 1) * (window_height / 2);
	vsc[2].x = vp3.x + (vsc[2].x + 1) * (window_width / 2);
	vsc[2].y = vp3.y + (vsc[2].y + 1) * (window_height / 2);
	// Since the Z axis goes from (0.1 to 700) 
	// TODO: Unable to use this as it clips all rect to triangles
	float vpz1 = (700 - 0.1) / 2;
	float vpz2 = (700 + 0.1) / 2;
	//vsc[0].z = vsc[0].z * vpz1 + vpz2;
	//vsc[1].z = vsc[1].z * vpz1 + vpz2;
	//vsc[2].z = vsc[2].z * vpz1 + vpz2;
	
}

/* Draw triangles in the frame buffer */
void Triangle::rasterizeTriangle(Matrix4 mvpMatrix, int id) {
	Vertex vsc[4];
	// Get screen coordinates
	if (!getScreenCoordinates(mvpMatrix, vsc))
		return;
	// Find the bounding box for the triangle
	float y_min = vsc[0].y, y_max = vsc[0].y,	x_min = vsc[0].x, x_max = vsc[0].x;
	y_min = max(min(min(vsc[0].y, vsc[1].y), vsc[2].y), 0);
	x_min = max(min(min(vsc[0].x, vsc[1].x), vsc[2].x), 0);
	x_max = min(max(max(vsc[0].x, vsc[1].x), vsc[2].x), window_width);
	y_max = min(max(max(vsc[0].y, vsc[1].y), vsc[2].y), window_height);

	/* Setup the Vertices of the triangles Texture */
	float color[3] = { 0, 255, 0 };
	float A = getArea(vsc[0], vsc[1], vsc[2]);
	float Ai = 1 / A;
	Vertex p(0, 0, 1);	  // Point inside triangle
	Vector3 pxy(0, 0, 0); // U, V tex coordinates
	int y = 0, x = 0, inCount = 0;
	// Basic inside triangle test 
	for (y = y_min; y <= y_max; y++) {
		for (x = x_min; x <= x_max; x++) {
			p.x = x;
			p.y = y;
			if (insideTriangle(p, vsc)) {
				if(tex){
					// Per pixel interpolation based on Barycentric coordinates 
					pxy = getTexInterpolation(p, vsc, Ai);
					// Map the texture 
					tex->getFloatColor(pxy.x, pxy.y, color); 
					fb.setColorBuffer(x, y, pxy.z, color);
				}else{
					Vector3 col = getColorInterpolation(p, vsc, Ai);
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

/* Per pixel texture interpolation based on Barycentric coordinates  */
Vector3 Triangle::getTexInterpolation(Vertex p, Vertex vp[4], float Ai) {
	Vector3 pxy(0, 0, 0);
	Vertex vtx[3]; // Texture coordinates
	vtx[0].set(coords[0][0], coords[0][1], 1);
	vtx[1].set(coords[1][0], coords[1][1], 1);
	vtx[2].set(coords[2][0], coords[2][1], 1);
	/* A0, A1, A2: three triangles formed around point P(x,y) */
	float A0 = getArea(p, vp[1], vp[2]);
	float A1 = getArea(p, vp[2], vp[0]);
	float A2 = getArea(p, vp[0], vp[1]);
	float wp = (A0 *Ai) * (vp[3].x) + (A1 *Ai) * (vp[3].y) + (A2 * Ai) * (vp[3].z);
	// Perspective correct texture interpolation based on Barycentric coordinates 
	pxy.x = tex->width  * ((A0 * Ai) * (vtx[0].x * vp[3].x) + (A1 * Ai) * (vtx[1].x * vp[3].y) + (A2 * Ai) * (vtx[2].x * vp[3].z)); // Px
	pxy.y = tex->height * ((A0 * Ai) * (vtx[0].y * vp[3].x) + (A1 * Ai) * (vtx[1].y * vp[3].y) + (A2 * Ai) * (vtx[2].y * vp[3].z)); // Py
	pxy.z = (A0 *Ai) * (vp[0].z *vp[3].x) + (A1 *Ai) * (vp[1].z * vp[3].y) + (A2 * Ai) * (vp[2].z * vp[3].z);
	pxy.x /= wp;
	pxy.y /= wp;
	pxy.z /= wp;
 	return pxy;
}
/* Color interpolation using Barymetric coordinates */
Vector3 Triangle::getColorInterpolation(Vertex p, Vertex vp[4], float Ai) {
	Vector3 color(0,0,0);
	float A0 = getArea(p, vp[1], vp[2]);
	float A1 = getArea(p, vp[2], vp[0]);
	float A2 = getArea(p, vp[0], vp[1]);
	// Perspective correct color interpolation based on Barycentric coordinates 
	color[0] = ((A0 * Ai) * (c[0][0] * vp[3].x) + (A1 * Ai) * (c[1][0] * vp[3].y) + (A2 * Ai) * (c[2][0] * vp[3].z)) * 255; // R
	color[1] = ((A0 * Ai) * (c[0][1] * vp[3].x) + (A1 * Ai) * (c[1][1] * vp[3].y) + (A2 * Ai) * (c[2][1] * vp[3].z)) * 255; // G
	color[2] = ((A0 * Ai) * (c[0][2] * vp[3].x) + (A1 * Ai) * (c[1][2] * vp[3].y) + (A2 * Ai) * (c[2][2] * vp[3].z)) * 255; // B
	float wp = (A0 *Ai) * (vp[3].x) + (A1 *Ai) * (vp[3].y) + (A2 * Ai) * (vp[3].z);
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
	// Obtain the area by cross product, we can drop the divide since this is used for ratios
	double dArea = ((v1.x - v0.x)*(v2.y - v0.y) - (v2.x - v0.x)*(v1.y - v0.y)) ; // / 2.0 
	return (float)(dArea > 0.0) ? dArea : -dArea;
}