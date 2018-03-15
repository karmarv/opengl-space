#ifndef LUTILS_H
#define LUTILS_H
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
#include <camera.hpp>
#include <filesystem.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void RenderBlock();
unsigned int loadTexture(char const * path);

#endif