#include "lightltc.hpp"
#include "ltc.h"


LightLtc::LightLtc()
{
}


LightLtc::~LightLtc()
{
}

GLfloat ltcVertices[] = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };


/*
	// bind roughness   {label:"Roughness", default:0.25, min:0.01, max:1, step:0.001}
	// bind dcolor      {label:"Diffuse Color",  r:1.0, g:1.0, b:1.0}
	// bind scolor      {label:"Specular Color", r:1.0, g:1.0, b:1.0}
	// bind intensity   {label:"Light Intensity", default:4, min:0, max:10}
	// bind width       {label:"Width",  default: 8, min:0.1, max:15, step:0.1}
	// bind height      {label:"Height", default: 8, min:0.1, max:15, step:0.1}
	// bind roty        {label:"Rotation Y", default: 0, min:0, max:1, step:0.001}
	// bind rotz        {label:"Rotation Z", default: 0, min:0, max:1, step:0.001}
	// bind twoSided    {label:"Two-sided", default:false}
*/
void LightLtc::Render(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
	// Initialize (if necessary)
	if (cubeVAO == 0)
	{
		Setup();
	}





	glUniform2f(glGetUniformLocation(lightingShader->Program, "resolution"), width, height);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "roughness"), roughness);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "dcolor"), dcolor[0], dcolor[1], dcolor[2]);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "scolor"), scolor[0], scolor[1], scolor[2]);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "intensity"), intensity);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "roty"), roty);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "rotz"), rotz);
	glUniform1i(glGetUniformLocation(lightingShader->Program, "twoSided"), twoSided);

	lightingShader->Use(); //Activate Light shader
	// Model/View/Projection
	model = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	const long double sysTimeMs = time(0) * 1000;
	glUniform1f(glGetUniformLocation(lightingShader->Program, "time"), (float)(sysTimeMs));
	
	
	// bind diffuse map texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ltc_mat_texture);
	glUniform1i(glGetUniformLocation(lightingShader->Program, "ltc_mat"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ltc_mag_texture);
	glUniform1i(glGetUniformLocation(lightingShader->Program, "ltc_mag"), 1);
	/*glBindFramebuffer(GL_FRAMEBUFFER, rttFramebuffer);*/
	if (g_sample_count == 0) {
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		
	}
	glUniform1i(glGetUniformLocation(lightingShader->Program, "sampleCount"), g_sample_count);
	g_sample_count += 8;
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	

	// Render geometry
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	
	blitShader->Use(); // Activate Blit shader
	glDisable(GL_BLEND);
	// Model/View/Projection
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 1.0f));
	//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
	float angle = 90.0f;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(blitShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(blitShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(blitShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// bind diffuse map texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rttTexture);
	glUniform1i(glGetUniformLocation(blitShader->Program, "tex"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glUniform2f(glGetUniformLocation(blitShader->Program, "resolution"), width, height);
	// Blit pass
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	//Finally
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LightLtc::Setup()
{
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ltcVertices), ltcVertices, GL_STATIC_DRAW);
	// Link vertex attributes
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	//glGenFramebuffers(1, &rttFramebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, rttFramebuffer);

	glGenTextures(1, &rttTexture);
	glBindTexture(GL_TEXTURE_2D, rttTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rttTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glBindTexture(GL_TEXTURE_2D, NULL);

	// bind ltc_mat_texture map texture
	glGenTextures(1, &ltc_mat_texture);
	glBindTexture(GL_TEXTURE_2D, ltc_mat_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, g_ltc_mat);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	

	// bind ltc_mag_texture map texture
	glGenTextures(1, &ltc_mag_texture);
	glBindTexture(GL_TEXTURE_2D, ltc_mag_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 64, 64, 0, GL_ALPHA, GL_FLOAT, g_ltc_mag);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// LTC Shader
	lightingShader = new Shader(FileSystem::getPath("Shaders/ltc-copy.vs").c_str(), FileSystem::getPath("Shaders/ltc-copy.fs").c_str());
	// Blit Shader
	blitShader = new Shader(FileSystem::getPath("Shaders/ltc_blit-copy.vs").c_str(), FileSystem::getPath("Shaders/ltc_blit-copy.fs").c_str());

}
