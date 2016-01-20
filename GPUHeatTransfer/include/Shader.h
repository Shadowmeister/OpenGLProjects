#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

class Shader
{
public:
	// the program ID
	GLuint Program;
	// Constructor reads the builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	Shader(const GLchar* computePath);
	Shader(const std::string& computePath);
		
	// Use the program
	void use();

	GLuint getUniformLocation(const GLchar* name) const;
};

#endif
