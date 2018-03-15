#ifndef BLOCKS_H
#define BLOCKS_H
#pragma once

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <shader.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void RenderBlock();
unsigned int loadTexture(char const * path);

#endif