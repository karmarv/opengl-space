#ifndef LIGHTLTC_H
#define LIGHTLTC_H
#pragma once

#include "lutils.hpp"
#include <ctime>

class LightLtc
{
public:
	LightLtc();
	virtual ~LightLtc();

	/* Shader */
	Shader *lightingShader;
	Shader *blitShader;

	/* Variables */
	// Set values to program variables
	const int width = 8;
	const int height = 8;
	float roughness = 0.25f;
	glm::vec3 dcolor{ 1.0f, 0.0f, 0.0f };
	glm::vec3 scolor{ 0.0f, 1.0f, 0.0f };
	float intensity = 4.0f;
	float roty = 0.0f;
	float rotz = 0.0f;
	bool twoSided = false;

	GLuint cubeVAO = 0;
	GLuint lightVAO = 0;
	GLuint VBO = 0;

	GLuint rttTexture = 0;
	GLuint rttFramebuffer = 0;
	

	//Texture objects 
	GLuint ltc_mat_texture = 0;
	GLuint ltc_mag_texture = 0;
	int g_sample_count = 0;
	
	/* Functions */
	void Render(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
	void Setup();


};

#endif

