#ifndef _MESH_H_
#define _MESH_H_

#include <vector>

#include "glm/glm.hpp"

#include "Texture.h"
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh {
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
	Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, std::vector<Texture>&& textures);
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other);
	void Draw(const Shader& shader);
private:
	/* Mesh data */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	/* Render data */
	GLuint VAO, VBO, EBO;

	/* Functions */
	void setupMesh();
};

#endif