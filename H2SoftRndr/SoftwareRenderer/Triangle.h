#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdlib.h>
#include <math.h>

#include "Matrices.h"
#include "Vertex.h"
#include "FrameBuffer.h"
#include "Texture.h"

//#define DEBUG_TEST


/* implements the triangle class */
class Triangle {
	private:
	public:
		Vertex v[3];		/* the original vertices of the triangle */
		Vertex pv[3];		/* the perspective transformed vertices of the triangle */
							/* per-vertex values */
		float c[3][3];		/* color at each vertex */
		float coords[3][2];

		/* point to the texture that is bound, if any */
		Texture *tex;


		/* constructors */
		Triangle() {
			v[0].set(0, 0, 0);
			v[1].set(0, 0, 0);
			v[2].set(0, 0, 0);

			pv[0].set(0, 0, 0);
			pv[1].set(0, 0, 0);
			pv[2].set(0, 0, 0);

			c[0][0] = 0.0;	c[0][1] = 0.0;	c[0][2] = 0.0;
			c[1][0] = 0.0;	c[1][1] = 0.0;	c[1][2] = 0.0;
			c[2][0] = 0.0;	c[2][1] = 0.0;	c[2][2] = 0.0;

			coords[0][0] = 0.0;		coords[0][1] = 0.0;	
			coords[1][0] = 0.0;		coords[1][1] = 0.0;	
			coords[2][0] = 0.0;		coords[2][1] = 0.0;

			tex = NULL;

		};

		Triangle(Vertex *v0, Vertex *v1, Vertex *v2) {
			v[0] = (*v0);
			v[1] = (*v1);
			v[2] = (*v2);

			pv[0] = (*v0);
			pv[1] = (*v1);
			pv[2] = (*v2);

			c[0][0] = 0.0;	c[0][1] = 0.0;	c[0][2] = 0.0;
			c[1][0] = 0.0;	c[1][1] = 0.0;	c[1][2] = 0.0;
			c[2][0] = 0.0;	c[2][1] = 0.0;	c[2][2] = 0.0;
			
			coords[0][0] = 0.0;		coords[0][1] = 0.0;		coords[0][2] = 0.0;
			coords[1][0] = 0.0;		coords[1][1] = 0.0;		coords[1][2] = 0.0;
			coords[2][0] = 0.0;		coords[2][1] = 0.0;		coords[2][2] = 0.0;

			tex = NULL;
		};


		/* sets the color of vertex v of the triangle to (r,g,b) */
		void setColor(int v, int r, int g, int b) {
			if ((r < 0) || (r > 255) ||
				(g < 0) || (g > 255) ||
				(b < 0) || (b > 255)) {
				fprintf(stderr,"ERROR! Invalid color set in triangle\n");
				fflush(stderr);
				exit(-1);
			}

			c[v][0] = ((float)r/(float)255);
			c[v][1] = ((float)g/(float)255);	
			c[v][2] = ((float)b/(float)255);
	
			return;
		};

		void setCoords(int v, int s, int t) {
			coords[v][0] = s;
			coords[v][1] = t;

			return;
		};

		void setTexture(Texture *t) {
			tex = t;
			return;
		}

		/* openGL rendering of the triangle */
		void renderOpenGL(void);

		/* Software rendering */
		void rasterizeTriangle();
		int getScreenCoordinates(Vector4 vp1, Vector4 vp2, Vector4 vp3, Vertex *vsc);
		void modelViewTransformVertices(Matrix4 mvpMatrix);


		/* Triangle Inside test functions */
		bool insideTriangle(Vertex p, Vertex vp[3]);
		float getArea(Vertex v0, Vertex v1, Vertex v2);
		void lineEquation(Vertex *f, Vertex *vector1, Vertex *vector2);
		float dotProduct(Vertex *v1, Vertex *v2);
		void crossProduct(Vertex *result, Vertex *vector1, Vertex *vector2);

		/* Interpolation function */
		Vector3 getColorInterpolation(Vertex p, Vertex vp[4], float Ai);
		Vector3 getTexInterpolation(Vertex p, Vertex vp[4], float Ai);

		/* for debugging */
		void print(void) {
			fprintf(stderr,"Triangle:\n");
			v[0].print();
			v[1].print();
			v[2].print();
			return;
		};
};

#endif		/* TRIANGLE_H */