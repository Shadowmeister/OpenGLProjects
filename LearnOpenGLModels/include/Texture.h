#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <memory>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <string>
#include <assimp/Importer.hpp>

struct TextureIDDeleter
{
	void operator()(GLuint* id)
	{
		std::cout << "Deleting Texture " << *id << std::endl;
		glDeleteTextures(1, id);
		delete id;
	}
};

class Texture
{
public:
	enum TextureTypes{
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		NORMAL,
		HEIGHT
	};

public:
	Texture(const std::string& texturePath, const std::string& directory, TextureTypes type, GLint wrapSParam, GLint wrapTParam, GLint minFilterParam, GLint magFilterParam);
	Texture(const Texture& other);
	Texture(Texture&& other) noexcept;

	Texture& operator=(const Texture& other);
	Texture& operator=(Texture&& other);

	~Texture();

	void Bind();
	GLuint GetTextureID();
	TextureTypes GetType();
	aiString GetPath();

private:
	std::shared_ptr<GLuint> m_TextureID;
	TextureTypes m_Type;
	aiString m_Path;
};

#endif
