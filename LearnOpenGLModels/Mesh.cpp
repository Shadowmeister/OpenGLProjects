#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->setupMesh();
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, std::vector<Texture>&& textures)
{
	this->vertices = std::move(vertices);
	this->indices = std::move(indices);
	this->textures = std::move(textures);

	this->setupMesh();
}

Mesh::Mesh(Mesh&& other) :
VAO{ std::move( other.VAO ) },
VBO{ std::move( other.VBO ) },
EBO{ std::move( other.EBO ) }
{
	this->vertices = std::move(other.vertices);
	this->indices = std::move(other.indices);
	this->textures = std::move(other.textures);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
  // Vertex Tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
  // Vertex Bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint heightNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)
		std::string name = "material.texture_";
		Texture::TextureTypes type = this->textures[i].GetType();
		if (type == Texture::DIFFUSE)
		{
			name += "diffuse";
			name += std::to_string(diffuseNr++);
		}
		else if (type == Texture::SPECULAR)
		{
			name += "specular";
			name += std::to_string(specularNr++);
		}
		else if (type == Texture::HEIGHT)
		{
			name += "height";
			name += std::to_string(heightNr++);
		}

		glUniform1i(shader.getUniformLocation(name.c_str()), i);
		this->textures[i].Bind();
	}

  if(heightNr == 1)
  {
    std::cout << "no normal map for this mesh" << std::endl;
    for(GLuint i = 0; i < this->textures.size(); i++)
    {
      std::cout << "\t" << this->textures[i].GetPath().C_Str() << std::endl;
    }
  }

	glUniform1f(shader.getUniformLocation("material.shininess"), 32.0f);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
