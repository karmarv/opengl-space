#pragma once

#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Shader{
public:
	unsigned int loadShader(const char *vertex_path, const char *fragment_path);

	Shader() { };
	Shader(const char *vertex_path, const char *fragment_path){
		this->vertexPath = vertex_path;
		this->fragmentPath = fragment_path;
		this->program = this->loadShader(vertexPath.c_str(), fragmentPath.c_str());
	}

	void reload() {
		this->program = this->loadShader(vertexPath.c_str(), fragmentPath.c_str());
	}


	operator unsigned int() { return program; }
private:
    unsigned int program;
		std::string vertexPath;
		std::string fragmentPath;
};
#endif
