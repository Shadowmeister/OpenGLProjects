#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <string>

class Texture
{
public:
	enum TextureTypes{
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};

public:
	Texture(const std::string& texturePath, TextureTypes type, GLint wrapSParam, GLint wrapTParam, GLint minFilterParam, GLint magFilterParam);
	~Texture();

	void Bind();
	GLuint GetTextureID();
	TextureTypes GetType();

private:
	GLuint m_TextureID;
	TextureTypes m_Type;
};

#endif