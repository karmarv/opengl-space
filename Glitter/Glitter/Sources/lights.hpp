
#ifndef LIGHTS_H
#define LIGHTS_H
#pragma once

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <shader.hpp>
#include <filesystem.hpp>
#include "blocks.hpp"



void RenderLightCube(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec3 cameraPos, glm::vec3 cameraFront);
void SetupLightCube();
void SetPointLights();

#endif
