#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <string>
#include <assimp/Importer.hpp>

class Texture
{
public:
	enum TextureTypes{
		DIFFUSE,
		SPECULAR,
		EMISSIVE
	};

public:
	Texture(const std::string& texturePath, const std::string& directory, TextureTypes type, GLint wrapSParam, GLint wrapTParam, GLint minFilterParam, GLint magFilterParam);
	Texture(const Texture& other);
	~Texture();

	void Bind();
	GLuint GetTextureID();
	TextureTypes GetType();
	aiString GetPath();

private:
	GLuint m_TextureID;
	TextureTypes m_Type;
	aiString m_Path;
};

#endif