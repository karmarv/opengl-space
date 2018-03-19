#ifndef LIGHTPOLY_H
#define LIGHTPOLY_H
#pragma once

#include "lutils.hpp"

class LightPoly
{
public:
	LightPoly();
	virtual ~LightPoly();


	/* Variables */
	Shader *lightingShader;

	// RenderCube() Renders a 1x1 3D cube in NDC.
	GLuint cubeVAO = 0;
	GLuint lightVAO = 0;
	GLuint VBO = 0;

	//Texture objects 
	unsigned int _ltcAmp = 0;
	unsigned int _ltcMat = 0;

	/* Functions */
	void Render(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
	void Setup();
};


#endif