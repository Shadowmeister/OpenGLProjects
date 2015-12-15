#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	// the program ID
	GLuint Program;
	// Constructor reads the builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
		
	// Use the program
	void use();

	GLuint getUniformLocation(const GLchar* name);
};

#endif
