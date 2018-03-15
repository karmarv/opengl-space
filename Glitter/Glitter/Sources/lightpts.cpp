// Local Headers
#include "lightpts.hpp"


// lighting
glm::vec3 lightPos(1.2f, 0.2f, 2.0f);
// RenderCube() Renders a 1x1 3D cube in NDC.
GLuint cubeVAO = 0;
GLuint lightVAO = 0;
GLuint VBO = 0;

// Create a sample shader that displays normal
Shader *lampShader;
Shader *lightingShader;

//Texture objects 
unsigned int diffuseMap = 0;
unsigned int specularMap = 0;

/* Vertices cube */
GLfloat cubeVertices[] = {
    // positions          // normals           // texture coords
	// Back face (pos x, y, z, normal x, y, z, tex coords x, y)
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,	// top-right
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,	// bottom-right
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,	// top-right
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,	// bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,	// top-left
	// Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  // bottom-left
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,	// bottom-right
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,	// top-right
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,	// top-right
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,	// top-left
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,	// bottom-left
	// Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // top-right
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,	// top-left
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// bottom-left
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// bottom-left
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,	// bottom-right
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// top-right
	// Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // top-left
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  // bottom-right
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // top-right    
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // bottom-right
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  // top-left
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // bottom-left  
	 // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 	// top-right
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,	// top-left
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,	// bottom-left
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,	// bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,	// bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,	// top-right
	// Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,	// top-left
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,	// bottom-right
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,	// top-right   
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,	// bottom-right
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,	// top-left
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f	// bottom-left 
};
// positions all containers
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
// Point light sources
glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};  

/** Methods **/

void SetupLightCube()
{
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// Link vertex attributes
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	
	// Initialize shaders
	lampShader = new Shader(FileSystem::getPath("Shaders/lamp.vert.glsl").c_str(), FileSystem::getPath("Shaders/lamp.frag.glsl").c_str());
	lightingShader = new Shader(FileSystem::getPath("Shaders/lighting.vert.glsl").c_str(), FileSystem::getPath("Shaders/lighting.frag.glsl").c_str());

	// Load textures 
	diffuseMap = loadTexture(FileSystem::getPath("Resources/textures/container2.png").c_str());
	specularMap = loadTexture(FileSystem::getPath("Resources/textures/container2_specular.png").c_str());

}

void RenderLightCube(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec3 cameraPos, glm::vec3 cameraFront)
{
	// Initialize (if necessary)
	if (lightVAO == 0)
	{
		SetupLightCube();
	}

	lightingShader->Use(); //Activate Light shader
	// Material
	glUniform1i(glGetUniformLocation(lightingShader->Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader->Program, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "material.shininess"), 32.0f);
	//Light Directional
	glUniform3f(glGetUniformLocation(lightingShader->Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
	//Light Point
	SetPointLights();
	glUniform3f(glGetUniformLocation(lightingShader->Program, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	//Spot Light
	glUniform3f(glGetUniformLocation(lightingShader->Program, "spotLight.position"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "spotLight.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "spotLight.linear"), 0.09);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "spotLight.quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(lightingShader->Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));


	// view/projection transformations
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// world transformation
	model = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// bind diffuse map texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	// bind specular map texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	// render containers
	glBindVertexArray(cubeVAO);
	for (unsigned int i = 0; i < 10; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model;
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// also draw the lamp object
	lampShader->Use();
	glUniformMatrix4fv(glGetUniformLocation(lampShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lampShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));	
	// Render Lamp
	glBindVertexArray(lightVAO);
	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(glGetUniformLocation(lampShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SetPointLights() {
	// point light 1
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[0].linear"), 0.09);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[0].quadratic"), 0.032);
	// point light 2
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[1].linear"), 0.09);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[1].quadratic"), 0.032);
	// point light 3
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[2].linear"), 0.09);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[2].quadratic"), 0.032);
	// point light 4
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(lightingShader->Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[3].linear"), 0.09);
	glUniform1f(glGetUniformLocation(lightingShader->Program, "pointLights[3].quadratic"), 0.032);
}

