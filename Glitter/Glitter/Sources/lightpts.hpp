
#ifndef LIGHTS_H
#define LIGHTS_H
#pragma once

#include "lutils.hpp"

void RenderLightCube(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec3 cameraPos, glm::vec3 cameraFront);
void SetupLightCube();
void SetPointLights();

#endif
