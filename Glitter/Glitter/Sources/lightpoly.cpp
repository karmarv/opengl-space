#include "lightpoly.hpp"



LightPoly::LightPoly()
{
}


LightPoly::~LightPoly()
{
}

struct {
	glm::vec3 position;
	glm::vec3 normal;
} LightVerticesStruct[] = {
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
};

GLfloat lightVertices[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

void LightPoly::Render(glm::mat4 projection, glm::mat4 view, glm::mat4 model, Camera camera)
{
	// Initialize (if necessary)
	if (lightVAO == 0)
	{
		Setup();
	}

	lightingShader->Use(); //Activate Light shader

	// Set the Uniform locations
		
	glm::mat4 _lightTransform[3]{
		glm::translate(model, glm::vec3(-3.25f, 2.0f, -2.1f)) *
		glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *   // Y
		glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *  // X
		glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *   // Z
		glm::scale(model, glm::vec3(0.5f, 1.75f, 1.0f)),

		glm::translate(model, glm::vec3(0.0f, 1.8f, -2.5f)) *
		glm::rotate(model, glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *   // X
		glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *   // Z
		glm::scale(model, glm::vec3(1.25f, 1.25f, 1.0f)),

		glm::translate(model, glm::vec3(3.25f, 2.0f, -2.1f)) *
		glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *  // Y
		glm::rotate(model, glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *	 // X
		glm::rotate(model, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *  // Z
		glm::scale(model, glm::vec3(0.75f, 1.25f, 1.0f))
		};
	float _lightColor[3]{ 1.0f, 1.0f, 1.0f };
	float _lightIntensity[3]{ 1.0f, 1.5f, 1.0f };
	bool _lightTwoSided[3]{ true, false, true };
	glUniform3f(glGetUniformLocation(lightingShader->Program, "u_baseColor"), _lightColor[0], _lightColor[1], _lightColor[2]);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "u_lightIntensity"), _lightIntensity[0], _lightIntensity[1], _lightIntensity[2]);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "u_twoSided"), _lightTwoSided[0], _lightTwoSided[1], _lightTwoSided[2]);
	//Camera
	glm::vec3 _cameraPosition{ 0.0f, 1.0f, 7.6f };
	glm::vec3 _cameraDirection;
	glm::vec2 _cameraRotation;

	/* Material properties */
	float _metalness = 0.5f;
	float _roughness = 0.25f;
	float _f0 = 0.5f; /* Specular reflection coefficient */
	glUniform1f(glGetUniformLocation(lightingShader->Program, "u_metalness"), _metalness);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "u_roughness"), _roughness);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "u_f0"), _f0);

	// layout(binding = 0) uniform sampler2D s_texLTCMat;
	// layout(binding = 1) uniform sampler2D s_texLTCAmp;
	glUniform1i(glGetUniformLocation(lightingShader->Program, "s_texLTCMat"), 0);
	glUniform1i(glGetUniformLocation(lightingShader->Program, "s_texLTCAmp"), 1);

	// Model/View/Projection
	model = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "u_transformationMatrix"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "u_projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "u_viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "u_normalMatrix"), 1, GL_FALSE, glm::value_ptr(model));
	// View Position
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "u_viewPosition"), 1, GL_FALSE, glm::value_ptr(view));

	// bind diffuse map texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _ltcAmp);
	// bind specular map texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _ltcMat);

	/*
		_lightQuadUniform = uniformLocation("u_quadPoints");
	*/

	// Render
	glBindVertexArray(lightVAO);
	for (unsigned int i = 0; i < 3; i++)
	{
		glm::vec3 quadPoints[4];
		for (unsigned int i = 0; i < 4; i++) {
			quadPoints[i] = _lightTransform[i] * glm::vec4(lightVertices[6 * i + 0], lightVertices[6 * i + 1], lightVertices[6 * i + 2], 1.0);
		}
		glUniform3f(glGetUniformLocation(lightingShader->Program, "u_quadPoints[0]"), quadPoints[0].x, quadPoints[0].y, quadPoints[0].z);
		glUniform3f(glGetUniformLocation(lightingShader->Program, "u_quadPoints[1]"), quadPoints[1].x, quadPoints[1].y, quadPoints[1].z);
		glUniform3f(glGetUniformLocation(lightingShader->Program, "u_quadPoints[2]"), quadPoints[2].x, quadPoints[2].y, quadPoints[2].z);
		glUniform3f(glGetUniformLocation(lightingShader->Program, "u_quadPoints[3]"), quadPoints[3].x, quadPoints[3].y, quadPoints[3].z);
		glDrawArrays(GL_TRIANGLES, 0, 4);
	}


	//Finally
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LightPoly::Setup()
{
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
	// Link vertex attributes
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));


	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	// Shader
	lightingShader = new Shader(FileSystem::getPath("Shaders/lightpoly.vert.glsl").c_str(), FileSystem::getPath("Shaders/lightpoly.frag.glsl").c_str());

	// Load textures 
	_ltcAmp = loadTexture(FileSystem::getPath("Resources/textures/container2.png").c_str()); //ltc_amp.dds
	_ltcMat = loadTexture(FileSystem::getPath("Resources/textures/container2_specular.png").c_str()); //ltc_mat.dds

}
