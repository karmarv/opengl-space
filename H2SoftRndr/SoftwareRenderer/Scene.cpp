#include "Scene.h"

extern FrameBuffer fb;

/* ModelView and Projection matrix variable*/
Matrix4 M;
Matrix4 P;

/* Mem Allocations */
Texture ** tx = (Texture **)malloc(sizeof(Texture*) * 4);

/* Compute dot product of vertices */
float dot(Vertex v1, float *v2) {
	return ((v1.x * v2[0]) + (v1.y * v2[1]) + (v1.z * v2[2]));
}

/* Compute the frustum matrix */
Matrix4 setFrustum(float l, float r, float b, float t, float n, float f){
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
Matrix4 setFrustum(float fovY, float aspect, float front, float back){
	const float DEG2RAD = 3.141593f / 180.0f;
	float tangent = tanf(fovY / 2 * DEG2RAD); // tangent of half fovY
	float height = front * tangent;         // half height of near plane
	float width = height * aspect;          // half width of near plane
											// params: left, right, bottom, top, near, far
	return setFrustum(-width, width, -height, height, front, back);
}

/* Projection: set the perspective projection matrix for obtaining clip coordinates with modelview */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float front, float back) {
	P.identity();
	P = setFrustum(eye_fov, aspect_ratio, front, back);
}


/* Modelview: set the modelview matrix with the given parameters for eye coordinate */
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

float Scene::getPtToPlaneW(float v1, float v2, float w1, float w2, int isWPositive) {
	// Computed in class for (w = x)
	float p = 0.0;
	if (isWPositive > 0) {
		float t = (v1 - w1) / (w2 - w1 + v1 - v2);
		p = (v1 + (v2 - v1) * t);
	}
	else
		p = (v2 * w1 - v1 * w2) / (w1 - w2 + v1 - v2);
	return p;
}

/*	Computed intersection factor with plane x,y,z = (+w) 
	Get the alpha factor for Axis (x=1, y=2, z=3)
*/
float Scene::getAlphaToPlaneW(Vector4 v1, Vector4 v2, int isWPositive, int axis) {
	float a = 0.0;
	if (axis == 1) { // X Axis
		if (isWPositive > 0)
			a = (v2.w - v2.x) / (v1.x - v2.x + v2.w - v1.w);
		else
			a = (-v2.w - v2.x) / (v1.x - v2.x + v1.w - v2.w);
	} else if (axis == 2) { // Y Axis
		if (isWPositive > 0)
			a = (v2.w - v2.y) / (v1.y - v2.y + v2.w - v1.w);
		else
			a = (-v2.w - v2.y) / (v1.y - v2.y + v1.w - v2.w);
	} else if (axis == 3) { // Z Axis
		if (isWPositive > 0)
			a = (v2.w - v2.z) / (v1.z - v2.z + v2.w - v1.w);
		else
			a = (-v2.w - v2.z) / (v1.z - v2.z + v1.w - v2.w);
	}
	return a;
}

/* Get the xyz for the computed alpha */
Vector4 Scene::getIntersectionToPlaneW(Vector4 v1, Vector4 v2, int isWPositive, float a) {
	float x = 0.0, y = 0.0, z = 0.0, w = 0.0;
	// Computed intersection with plane x,y,z = (-w)
	if (isWPositive > 0) {
		x = a * v1.x + (1 - a) * v2.x;
		y = a * v1.y + (1 - a) * v2.y;
		z = a * v1.z + (1 - a) * v2.z;
		w = a * v1.w + (1 - a) * v2.w;
	}
	else {
		x = a * v1.x + (1 - a) * v2.x;
		y = a * v1.y + (1 - a) * v2.y;
		z = a * v1.z + (1 - a) * v2.z;
		w = a * v1.w + (1 - a) * v2.w;
	}
	Vector4 p(x, y, z, w);
	return p;
}

/* Interpolate the texture coordinates
 */
Vector4 Scene::getTexCoordToPlaneW(float x1, float y1, float x2, float y2, int isWPositive, float a) {
	float x = 0.0, y = 0.0, z = 0.0, w = 0.0;
	// Computed intersection with plane x,y,z = (-w)
	if (isWPositive > 0) {
		x = a * x1 + (1 - a) * x2;
		y = a * y1 + (1 - a) * y2;
	}
	else {
		x = a * x1 + (1 - a) * x2;
		y = a * y1 + (1 - a) * y2;
	}
	Vector4 p(x, y, z, w);
	return p;
}

int Scene::clipTriangleX(Triangle *tr) {
	// Vertex Vectors
	Vector4 vp1(tr->pv[0].x, tr->pv[0].y, tr->pv[0].z, tr->pv[0].w);
	Vector4 vp2(tr->pv[1].x, tr->pv[1].y, tr->pv[1].z, tr->pv[1].w);
	Vector4 vp3(tr->pv[2].x, tr->pv[2].y, tr->pv[2].z, tr->pv[2].w);

	/* Check VP1 is outside +/-W */
	int isVp1OutP = 0, isVp1OutN = 0;
	if (vp1.x > vp1.w) 	isVp1OutP++;
	else if (vp1.x > -vp1.w) isVp1OutN++;

	/* Check VP2 is outside  +/-W */
	int isVp2OutP = 0, isVp2OutN = 0;
	if (vp2.x > vp2.w)	isVp2OutP++;
	else if (vp2.x > -vp2.w) isVp2OutN++;
	
	/* Check VP3 is outside  +/-W */
	int isVp3OutP = 0, isVp3OutN = 0;
	if (vp3.x > vp3.w)	isVp3OutP++;
	else if (vp3.x > -vp3.w) isVp3OutN++;

	int isPositiveWAxis = 1;

	// In case 2 vertices are out on X = + W
	if ((isVp1OutP + isVp2OutP + isVp3OutP) == 2) {

		Triangle *t1 = new Triangle(&tr->v[0], &tr->v[1], &tr->v[2]);  // Add T1
		t1->setColor(0, tr->c[0][0], tr->c[0][1], tr->c[0][2]);
		t1->setColor(1, tr->c[1][0], tr->c[1][1], tr->c[1][2]);
		t1->setColor(2, tr->c[2][0], tr->c[2][1], tr->c[2][2]);
		// Two vertices are outside the view
		if (isVp1OutP * isVp2OutP == 1) {		//V1 & V2 are out
			float a = getAlphaToPlaneW(vp3, vp1, isPositiveWAxis, 1), b = getAlphaToPlaneW(vp3, vp2, isPositiveWAxis, 1);
			Vector4 p13 = getIntersectionToPlaneW(vp3, vp1, isPositiveWAxis, a);  // p13 x,y,z = +w
			Vector4 p23 = getIntersectionToPlaneW(vp3, vp2, isPositiveWAxis, b);  // p23 x,y,z = +w
			// Triangle 1
			Vertex *t1v1 = new Vertex(vp3.x, vp3.y, vp3.z, vp3.w); //V3 
			Vertex *t1v2 = new Vertex(p13.x, p13.y, p13.z, p13.w); //V13
			Vertex *t1v3 = new Vertex(p23.x, p23.y, p23.z, p23.w); //V23
			t1->pv[0] = *t1v1; t1->pv[1] = *t1v2; t1->pv[2] = *t1v3;
			t1->setTexture(tr->tex);
			Vector4 tp31 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, a); //1-3
			Vector4 tp23 = getTexCoordToPlaneW(tr->coords[1][0], tr->coords[1][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, b); //2-3
			t1->setCoords(0, tr->coords[2][0], tr->coords[2][1]); //V3  
			t1->setCoords(1, tp31.x, tp31.y);					  //V13
			t1->setCoords(2, tp23.x, tp23.y);					  //V23
		}
		else if (isVp2OutP * isVp3OutP == 1) { //V2 & V3 are out 
			float a = getAlphaToPlaneW(vp1, vp2, isPositiveWAxis, 1), b = getAlphaToPlaneW(vp1, vp3, isPositiveWAxis, 1);
			Vector4 p21 = getIntersectionToPlaneW(vp1, vp2, isPositiveWAxis, a);  // p21 x,y,z = +w
			Vector4 p31 = getIntersectionToPlaneW(vp1, vp3, isPositiveWAxis, b);  // p31 x,y,z = +w
			// Triangle 1
			Vertex *t1v1 = new Vertex(vp1.x, vp1.y, vp1.z, vp1.w); //V1 
			Vertex *t1v2 = new Vertex(p21.x, p21.y, p21.z, p21.w); //V21
			Vertex *t1v3 = new Vertex(p31.x, p31.y, p31.z, p31.w); //V31
			t1->pv[0] = *t1v1; t1->pv[1] = *t1v2; t1->pv[2] = *t1v3;
			t1->setTexture(tr->tex);
			Vector4 tp21 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[1][0], tr->coords[1][1], isPositiveWAxis, a); //2-1
			Vector4 tp31 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, b); //3-1
			t1->setCoords(0, tr->coords[0][0], tr->coords[0][1]); //V1  
			t1->setCoords(1, tp21.x, tp21.y);					  //V21
			t1->setCoords(2, tp31.x, tp31.y);					  //V31
		}
		else if (isVp3OutP * isVp1OutP == 1) { //V3 & V1 are out 
			float a = getAlphaToPlaneW(vp2, vp3, isPositiveWAxis, 1), b = getAlphaToPlaneW(vp2, vp1, isPositiveWAxis, 1);
			Vector4 p32 = getIntersectionToPlaneW(vp2, vp3, isPositiveWAxis, a);  // p32 x,y,z = +w
			Vector4 p12 = getIntersectionToPlaneW(vp2, vp1, isPositiveWAxis, b);  // p12 x,y,z = +w
			// Triangle 1
			Vertex *t1v1 = new Vertex(vp2.x, vp2.y, vp2.z, vp2.w); //V2 
			Vertex *t1v2 = new Vertex(p32.x, p32.y, p32.z, p32.w); //V32
			Vertex *t1v3 = new Vertex(p12.x, p12.y, p12.z, p12.w); //V12
			t1->pv[0] = *t1v1; t1->pv[1] = *t1v2; t1->pv[2] = *t1v3;
			t1->setTexture(tr->tex);
			Vector4 tp32 = getTexCoordToPlaneW(tr->coords[1][0], tr->coords[1][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, a); //3-2
			Vector4 tp12 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[1][0], tr->coords[1][1], isPositiveWAxis, b); //1-2
			t1->setCoords(0, tr->coords[1][0], tr->coords[1][1]); //V2
			t1->setCoords(1, tp32.x, tp32.y);					  //V32
			t1->setCoords(2, tp12.x, tp12.y);					  //V12
		}
		addTriangle(&clipped_head, &clipped_tail, t1);
	}
	else if ((isVp1OutP + isVp2OutP + isVp3OutP) == 1) {
		Triangle *t1 = new Triangle(&tr->v[0], &tr->v[1], &tr->v[2]);  // Add T1
		t1->setColor(0, tr->c[0][0], tr->c[0][1], tr->c[0][2]);
		t1->setColor(1, tr->c[1][0], tr->c[1][1], tr->c[1][2]);
		t1->setColor(2, tr->c[2][0], tr->c[2][1], tr->c[2][2]);
		Triangle *t2 = new Triangle(&tr->v[0], &tr->v[1], &tr->v[2]);  // Add T2
		t2->setColor(0, tr->c[0][0], tr->c[0][1], tr->c[0][2]);
		t2->setColor(1, tr->c[1][0], tr->c[1][1], tr->c[1][2]);
		t2->setColor(2, tr->c[2][0], tr->c[2][1], tr->c[2][2]);

		// One vertex is outside the view
		if (isVp1OutP == 1) {
			float a = getAlphaToPlaneW(vp1, vp2, isPositiveWAxis, 1), b = getAlphaToPlaneW(vp1, vp3, isPositiveWAxis, 1);
			Vector4 p21 = getIntersectionToPlaneW(vp1, vp2, isPositiveWAxis, a);  // p21 x,y,z = +w
			Vector4 p31 = getIntersectionToPlaneW(vp1, vp3, isPositiveWAxis, b);  // p31 x,y,z = +w
			// Triangle 1
			Vertex *t1v1 = new Vertex(vp3.x, vp3.y, vp3.z, vp3.w); //V3 
			Vertex *t1v2 = new Vertex(p31.x, p31.y, p31.z, p31.w); //V31
			Vertex *t1v3 = new Vertex(vp2.x, vp2.y, vp2.z, vp2.w); //V2
			t1->pv[0] = *t1v1; t1->pv[1] = *t1v2; t1->pv[2] = *t1v3;
			t1->setTexture(tr->tex);
			Vector4 tp31 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, b); //1-3
			t1->setCoords(0, tr->coords[2][0], tr->coords[2][1]); //V3  
			t1->setCoords(1, tp31.x, tp31.y);					  //V31
			t1->setCoords(2, tr->coords[1][0], tr->coords[1][1]); //V2
			// Triangle 2
			Vertex *t2v1 = new Vertex(p31.x, p31.y, p31.z, p31.w); //V31
			Vertex *t2v2 = new Vertex(p21.x, p21.y, p21.z, p21.w); //V21
			Vertex *t2v3 = new Vertex(vp2.x, vp2.y, vp2.z, vp2.w); //V2
			t2->pv[0] = *t2v1; t2->pv[1] = *t2v2; t2->pv[2] = *t2v3;
			t2->setTexture(tr->tex);
			Vector4 tp21 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[1][0], tr->coords[1][1], isPositiveWAxis, a); //1-2
			t2->setCoords(0, tp31.x, tp31.y);					  //V31
			t2->setCoords(1, tp21.x, tp21.y);					  //V21
			t2->setCoords(2, tr->coords[1][0], tr->coords[1][1]); //V2
		}
		else if (isVp2OutP == 1) {	//V2 is out
			float a = getAlphaToPlaneW(vp2, vp3, isPositiveWAxis, 1), b = getAlphaToPlaneW(vp2, vp1, isPositiveWAxis, 1);
			Vector4 p32 = getIntersectionToPlaneW(vp2, vp3, isPositiveWAxis, a);  // p32 x,y,z = +w
			Vector4 p12 = getIntersectionToPlaneW(vp2, vp1, isPositiveWAxis, b);  // p12 x,y,z = +w
			// Triangle 1
			Vertex *t1v1 = new Vertex(vp1.x, vp1.y, vp1.z, vp1.w); //V1 
			Vertex *t1v2 = new Vertex(p12.x, p12.y, p12.z, p12.w); //V12
			Vertex *t1v3 = new Vertex(vp3.x, vp3.y, vp3.z, vp3.w); //V3
			t1->pv[0] = *t1v1; t1->pv[1] = *t1v2; t1->pv[2] = *t1v3;
			t1->setTexture(tr->tex);
			Vector4 tp12 = getTexCoordToPlaneW(tr->coords[0][0], tr->coords[0][1], tr->coords[1][0], tr->coords[1][1], isPositiveWAxis, b); //1-2
			t1->setCoords(0, tr->coords[0][0], tr->coords[0][1]); //V1  
			t1->setCoords(1, tp12.x, tp12.y);					  //V12
			t1->setCoords(2, tr->coords[2][0], tr->coords[2][1]); //V3
			// Triangle 2
			Vertex *t2v1 = new Vertex(p12.x, p12.y, p12.z, p12.w); //V12
			Vertex *t2v2 = new Vertex(p32.x, p32.y, p32.z, p32.w); //V32
			Vertex *t2v3 = new Vertex(vp3.x, vp3.y, vp3.z, vp3.w); //V3
			t2->pv[0] = *t2v1; t2->pv[1] = *t2v2; t2->pv[2] = *t2v3;
			t2->setTexture(tr->tex);
			Vector4 tp32 = getTexCoordToPlaneW(tr->coords[1][0], tr->coords[1][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, a); //2-3
			t2->setCoords(0, tp12.x, tp12.y);					  //V12
			t2->setCoords(1, tp32.x, tp32.y);					  //V32
			t2->setCoords(2, tr->coords[2][0], tr->coords[2][1]); //V3
		}
		else if (isVp3OutP == 1) {	//V3 is out
			float a = getAlphaToPlaneW(vp3, vp1, isPositiveWAxis, 1), b = getAlphaToPlaneW(vp3, vp2, isPositiveWAxis, 1);
			Vector4 p13 = getIntersectionToPlaneW(vp3, vp1, isPositiveWAxis, a);  // p13 x,y,z = +w
			Vector4 p23 = getIntersectionToPlaneW(vp3, vp2, isPositiveWAxis, b);  // p23 x,y,z = +w
			// Triangle 1
			Vertex *t1v1 = new Vertex(vp2.x, vp2.y, vp2.z, vp2.w); //V2 
			Vertex *t1v2 = new Vertex(p23.x, p23.y, p23.z, p23.w); //V23
			Vertex *t1v3 = new Vertex(vp1.x, vp1.y, vp1.z, vp1.w); //V1
			t1->pv[0] = *t1v1; t1->pv[1] = *t1v2; t1->pv[2] = *t1v3;
			t1->setTexture(tr->tex);
			Vector4 tp23 = getTexCoordToPlaneW(tr->coords[1][0], tr->coords[1][1], tr->coords[2][0], tr->coords[2][1], isPositiveWAxis, b); //2-3
			t1->setCoords(0, tr->coords[1][0], tr->coords[1][1]); //V2  
			t1->setCoords(1, tp23.x, tp23.y);					  //V23
			t1->setCoords(2, tr->coords[0][0], tr->coords[0][1]); //V1
			// Triangle 2
			Vertex *t2v1 = new Vertex(p23.x, p23.y, p23.z, p23.w); //V23
			Vertex *t2v2 = new Vertex(p13.x, p13.y, p13.z, p13.w); //V13
			Vertex *t2v3 = new Vertex(vp1.x, vp1.y, vp1.z, vp1.w); //V1
			t2->pv[0] = *t2v1; t2->pv[1] = *t2v2; t2->pv[2] = *t2v3;
			t2->setTexture(tr->tex);
			Vector4 tp13 = getTexCoordToPlaneW(tr->coords[2][0], tr->coords[2][1], tr->coords[0][0], tr->coords[0][1], isPositiveWAxis, a); //3-1
			t2->setCoords(0, tp23.x, tp23.y);					  //V23
			t2->setCoords(1, tp13.x, tp13.y);					  //V13
			t2->setCoords(2, tr->coords[0][0], tr->coords[0][1]); //V1
		}
		addTriangle(&clipped_head, &clipped_tail, t1);
		addTriangle(&clipped_head, &clipped_tail, t2);
	}

	// In case 2 vertices are out on X = -W
	if ((isVp1OutP + isVp2OutP + isVp3OutP) == 2) {
		// Two vertices are outside the view
		if (isVp1OutN * isVp2OutN == 1) {	   //V1 & V2 are out

		}
		else if (isVp2OutN * isVp3OutN == 1) { //V2 & V3 are out 

		}
		else if (isVp3OutN * isVp1OutN == 1) { //V3 & V1 are out 

		}
	}
	else if ((isVp1OutN + isVp2OutN + isVp3OutN) == 1) {
		// One vertex is outside the view
		if (isVp1OutN == 1) {

		}
		else if (isVp2OutN == 1) {	//V2 is out

		}
		else if (isVp3OutN == 1) {	//V3 is out

		}
	}

	return (isVp1OutP + isVp2OutP + isVp3OutP ); //isVp1OutN + isVp2OutN + isVp3OutN
}

/*
Clipping triangle against the
x = ±w,	y = ±w, z = ±w  planes
*/
int Scene::clipTriangle(Triangle *tr) {
	// Vertex Vectors
	Vector4 vp1(tr->pv[0].x, tr->pv[0].y, tr->pv[0].z, tr->pv[0].w);
	Vector4 vp2(tr->pv[1].x, tr->pv[1].y, tr->pv[1].z, tr->pv[1].w);
	Vector4 vp3(tr->pv[2].x, tr->pv[2].y, tr->pv[2].z, tr->pv[2].w);

	/*
		CCW Vertices
		   v1/\(R)
		e21 /  \  e13
		   /    \
	(G)v2 /______\v3 (B)
			e32
	*/
	if (clipTriangleX(tr) != 0) {

	}
	else {
		Triangle *nt = new Triangle(&tr->v[0], &tr->v[1], &tr->v[2]);
		nt->pv[0] = tr->pv[0]; nt->pv[1] = tr->pv[1]; nt->pv[2] = tr->pv[2];
		nt->setTexture(tr->tex);
		nt->setCoords(0, tr->coords[0][0], tr->coords[0][1]);
		nt->setCoords(1, tr->coords[1][0], tr->coords[1][1]);
		nt->setCoords(2, tr->coords[2][0], tr->coords[2][1]);
		nt->setColor(0, tr->c[0][0], tr->c[0][1], tr->c[0][2]);
		nt->setColor(1, tr->c[1][0], tr->c[1][1], tr->c[1][2]);
		nt->setColor(2, tr->c[2][0], tr->c[2][1], tr->c[2][2]);
		// All of it is outside the view. Just add this triangle to the Clip list 
		addTriangle(&clipped_head, &clipped_tail, nt);
	}
	return 0;
}

int initval = 0;
/*	NOTE: Test Code
	1. Test triangles (Order is critical, otherwise the inside triangle logic doesnt work properly.)
	2. The vertex coordinate should be in homogeneous coordinates for MVP computation.
*/
void Scene::testRenderSceneSoftware(void){
	initval++;
	clipped_head = NULL; 		clipped_tail = NULL;
	test_tr_head = NULL; 		test_tr_tail = NULL;
	
	/* Clip Coordinates: Final View Matrix */
	Matrix4 mvpMatrix = P * M;
	tx[0] = new Texture("wall1.tif");
	tx[1] = new Texture("wall4.tif");
	tx[2] = new Texture("wall6.tif");
	tx[3] = new Texture("monaLisa.tif");

	// Lets set the triangles object coordinates in world space
	Vertex v1_t = { -20.0f,80.0f,0.0f,1.0f };
	Vertex v2_t = { -30.0f,70.0f,0.0f,1.0f };
	Vertex v3_t = { -30.0f,60.0f,30.0f,1.0f };
	Triangle *test = new Triangle(&v1_t, &v2_t, &v3_t);
	test->setCoords(0, 0, 0);
	test->setCoords(1, 1, 1);
	test->setCoords(2, 0, 1);
	test->setTexture(tx[0]);
	test->setColor(0, 127, 0, 0); // color[0] = (150 * id) % 255; // rand() %
	test->setColor(1, 0, 255, 0);
	test->setColor(2, 127, 0, 0);
	addTriangle(&test_tr_head, &test_tr_tail, test);
	
	v1_t = { -20.0f,70.0f,20.0f,1.0f };
	v2_t = { -30.0f,60.0f,20.0f,1.0f };
	v3_t = { -30.0f,50.0f,0.0f,1.0f };
	Triangle *test0 = new Triangle(&v1_t, &v2_t, &v3_t);
	
	test0->setCoords(0, 1, 1);
	test0->setCoords(1, 0, 1);
	test0->setCoords(2, 0, 0);
	test0->setTexture(tx[1]);
	test0->setColor(0, 0, 127, 0);
	test0->setColor(1, 0, 0, 255);
	test0->setColor(2, 0, 127, 0);
	addTriangle(&test_tr_head, &test_tr_tail, test0);

	v1_t = { -40.0f,80.0f,20.0f,1.0f };
	v2_t = { -70.0f,80.0f,20.0f,1.0f };
	v3_t = { -20.0f,80.0f,40.0f,1.0f };
	Triangle *test1 = new Triangle(&v1_t, &v2_t, &v3_t);
	test1->setCoords(0, 0, 0);
	test1->setCoords(1, 1, 1);
	test1->setCoords(2, 0, 1);
	test1->setTexture(tx[2]);
	test1->setColor(0, 0, 0, 127);
	test1->setColor(1, 255, 0, 0);
	test1->setColor(2, 0, 0, 127);
	addTriangle(&test_tr_head, &test_tr_tail, test1);
	
	v1_t = { 0.0f,0.0f,1.0f,1.0f };
	v2_t = { 1.0f,1.0f,1.0f,1.0f };
	v3_t = { 0.0f,1.0f,1.0f,1.0f };
	Triangle *test3 = new Triangle(&v1_t, &v2_t, &v3_t);
	test3->setCoords(0, 0, 0);
	test3->setCoords(1, 1, 1);
	test3->setCoords(2, 0, 1);
	test3->setTexture(tx[3]);
	test3->setColor(0, 255, 0, 0);
	test3->setColor(1, 0, 255, 0);
	test3->setColor(2, 0, 0, 255);
	addTriangle(&test_tr_head, &test_tr_tail, test3);

	/* Now use the linked list to draw */
	int count = 0;
	TriangleList *ptr;

	// Clip all the triangles
	for (ptr = test_tr_head; ptr;) {
		count++;
		ptr->t->modelViewTransformVertices(mvpMatrix);
		clipTriangle(ptr->t);
		ptr = ptr->next;
	}

	TriangleList *clp_ptr;
	int triangleCount = 0;
	// Render the clipped triangles
	for (clp_ptr = clipped_head; clp_ptr;) {
		if(clp_ptr != NULL){
			triangleCount++;
			clp_ptr->t->rasterizeTriangle();
			clp_ptr = clp_ptr->next;
		}
	}
	destroyList(&test_tr_head, &test_tr_tail);
	destroyList(&clipped_head, &clipped_tail);
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

#ifdef DEBUG_TEST
	/*Test with static triangles */
	testRenderSceneSoftware();

#endif // DEBUG_TEST

#ifndef DEBUG_TEST
	int count = 0;
	TriangleList *ptr;
	clipped_head = NULL; clipped_tail = NULL;
	/* now render the triangles in the list */
	for (ptr = original_head; ptr;) {
		count++;
		ptr->t->modelViewTransformVertices(mvpMatrix);
		clipTriangle(ptr->t);
		ptr = ptr->next;
		//if (count > 45)
		//	break;
	}

	TriangleList *clp_ptr;
	int triangleCount = 0;
	// Render the clipped triangles
	for (clp_ptr = clipped_head; clp_ptr;) {
		if (clp_ptr != NULL) {
			triangleCount++;
			clp_ptr->t->rasterizeTriangle();
			clp_ptr = clp_ptr->next;
		}
	}

	destroyList(&clipped_head, &clipped_tail);

#endif // !DEBUG_TEST

	
	return;
}