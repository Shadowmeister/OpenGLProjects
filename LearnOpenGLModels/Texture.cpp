#include "Texture.h"
#include <iostream>


Texture::Texture(const std::string& texturePath, const std::string& directory, TextureTypes type, GLint wrapSParam, GLint wrapTParam, GLint minFilterParam, GLint magFilterParam) :
	m_Type{ type }
{
	unsigned char* image;
	int width, height;

	m_Path = texturePath;

	m_TextureID.reset(new GLuint, TextureIDDeleter());

	glGenTextures(1, m_TextureID.get());

	image = SOIL_load_image((directory + "/" + texturePath).c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, *m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterParam);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const Texture& other):
m_Path {other.m_Path},
m_TextureID{ other.m_TextureID },
m_Type{ other.m_Type }
{
}

Texture::Texture(Texture&& other) :
m_Path{ std::move(other.m_Path) },
m_TextureID{ std::move(other.m_TextureID) },
m_Type{ std::move(other.m_Type) }
{
}

Texture& Texture::operator=(const Texture& other)
{
	this->m_Path = other.m_Path;
	this->m_TextureID = other.m_TextureID;
	this->m_Type = other.m_Type;

	return *this;
}

Texture& Texture::operator=(Texture&& other)
{
	this->m_Path = other.m_Path;
	this->m_TextureID = other.m_TextureID;
	this->m_Type = other.m_Type;

	other.m_Path = "";
	other.m_TextureID = NULL;
	other.m_Type = Texture::DIFFUSE;

	return *this;
}


Texture::~Texture()
{
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, *m_TextureID);
}

GLuint Texture::GetTextureID()
{
	return *m_TextureID;
}

Texture::TextureTypes Texture::GetType()
{
	return m_Type;
}

aiString Texture::GetPath()
{
	return m_Path;
}
