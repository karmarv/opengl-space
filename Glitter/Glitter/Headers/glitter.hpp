// Preprocessor Directives
#ifndef GLITTER
#define GLITTER
#pragma once

// System Headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <btBulletDynamicsCommon.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

//// Helper functions
#include <camera.hpp>
#include <shader.hpp>
#include <model.hpp>
#include <filesystem.hpp>


// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
//     #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

// Define Some Constants
const int mWidth = 1600;
const int mHeight = 1200;
const float mNear = 0.1f;
const float mFar = 1000.f;

#endif //~ Glitter Header
