#include "Texture.h"
#include <iostream>


Texture::Texture(const std::string& texturePath, TextureTypes type, GLint wrapSParam, GLint wrapTParam, GLint minFilterParam, GLint magFilterParam) :
	m_Type{ type }
{
	unsigned char* image;
	int width, height;

	glGenTextures(1, &m_TextureID);

	image = SOIL_load_image("Textures/container2.png", &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterParam);
}


Texture::~Texture()
{
	std::cout << "Deleting Texture " << m_TextureID << std::endl;
	glDeleteTextures(1, &m_TextureID);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

GLuint Texture::GetTextureID()
{
	return m_TextureID;
}

Texture::TextureTypes Texture::GetType()
{
	return m_Type;
}
