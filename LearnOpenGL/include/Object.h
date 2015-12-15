#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <string>
#include <memory>
#include <vector>
#include "Shader.h"

struct VBODeleter {
	void operator()(GLuint* vbo) const {
		std::cout << "Deleting VBO: " << vbo << std::endl;
		glDeleteBuffers(1, vbo);
	}
};

struct Attrib
{
	bool enabled;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLint stride;
	GLvoid* offset;
};

class Object
{
public:
	Object(GLfloat* bufferArray, size_t sizeofBuffer, GLuint numVertices, std::shared_ptr<Shader>& shader);

	Object(const Object& other);

	Object(const Object& other, std::shared_ptr<Shader>& shader);

	virtual ~Object();

	int AddAttrib(GLint size, GLenum type, GLboolean normalized, GLint stride, GLint offset);

	void EnableAttrib(int location, bool enable);

	void UpdateAttribs();

	void Draw();

	const Shader& GetShader();

	void SetShader(std::shared_ptr<Shader>& shader);

	const GLuint GetVBO();

	const void BindVAO();

	const void UnbindVAO();

private:
	std::shared_ptr<Shader> m_Shader;
	float* m_BufferArray;
	GLuint VAO;
	std::shared_ptr<GLuint> VBO;
	std::vector<Attrib> m_Attribs;
	bool m_AttribsUpdated;
	GLuint m_NumVertices;
};

#endif
