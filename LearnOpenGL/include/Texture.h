#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <string>

class Texture
{
public:
	Texture(const std::string& texturePath, GLint wrapSParam, GLint wrapTParam, GLint minFilterParam, GLint magFilterParam);
	~Texture();

	void Bind();
	GLuint GetTextureID();

private:
	GLuint m_TextureID;
};

#endif