<<<<<<< HEAD:LearnOpenGL/Object.cpp
#include <Object.h>

Object::Object(GLfloat* bufferArray, size_t sizeofBuffer, GLuint numVertices, std::shared_ptr<Shader>& shader) : 
	m_BufferArray{ bufferArray }, 
	m_Shader{ shader }
{
	glGenVertexArrays(1, &VAO);
	VBO.reset(new GLuint, VBODeleter());
	glGenBuffers(1, VBO.get());

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	std::cout << "sizeof(bufferArray) = " << sizeofBuffer << std::endl;
	glBufferData(GL_ARRAY_BUFFER, sizeofBuffer, bufferArray, GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_AttribsUpdated = false;

	m_NumVertices = numVertices;
}

Object::Object(const Object& other) : 
	VBO{ other.VBO }, 
	m_NumVertices{ other.m_NumVertices }, 
	m_Shader{ other.m_Shader }, 
	m_BufferArray{other.m_BufferArray}
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glBindVertexArray(0);

	m_AttribsUpdated = false;
	m_Attribs = other.m_Attribs;
}

Object::Object(const Object& other, std::shared_ptr<Shader>& shader) :
  VBO{ other.VBO },
  m_NumVertices{ other.m_NumVertices },
  m_Shader{ shader },
  m_BufferArray{ other.m_BufferArray }
{
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBindVertexArray(0);

  m_AttribsUpdated = false;
  m_Attribs = std::vector<Attrib>();
}

Object::~Object()
{
	glDeleteVertexArrays(1, &VAO);
	VBO.reset();
}

int Object::AddAttrib(GLint size, GLenum type, GLboolean normalized, GLint stride, GLint offset)
{
	m_Attribs.emplace_back(Attrib{ true, size, type, normalized, stride, (GLvoid*)(long)offset });
	m_AttribsUpdated = false;
	return m_Attribs.size();
}

void Object::EnableAttrib(int location, bool enable)
{
	location--;
	if (location < m_Attribs.size() && location >= 0)
	{
		if (m_Attribs[location].enabled != enable)
		{
			m_Attribs[location].enabled = enable;
			m_AttribsUpdated = false;
		}
	}
	else
	{
		std::cout << "Location " << location + 1 << "Does not exist" << std::endl;
	}

}

void Object::UpdateAttribs()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	for (int i = 0; i < m_Attribs.size(); i++)
	{
		if (m_Attribs[i].enabled)
		{
			glVertexAttribPointer(i,
				m_Attribs[i].size,
				m_Attribs[i].type,
				m_Attribs[i].normalized,
				m_Attribs[i].stride,
				m_Attribs[i].offset);
			glEnableVertexAttribArray(i);
		}
		else
		{
			glDisableVertexAttribArray(i);
		}
	}
	glBindVertexArray(0);
}

void Object::Draw()
{
	if (!m_AttribsUpdated)
	{
		UpdateAttribs();
	}
	m_Shader->use();
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);

	glBindVertexArray(0);
}

const Shader& Object::GetShader()
{
	return *m_Shader.get();
}

void Object::SetShader(std::shared_ptr<Shader>& shader)
{
  m_Shader = shader;
}

const GLuint Object::GetVBO()
{
	return *VBO.get();
}

const void Object::BindVAO()
{
	glBindVertexArray(VAO);
}

const void Object::UnbindVAO()
{
	glBindVertexArray(0);
}
=======
#include <SimpleObject.h>

SimpleObject::SimpleObject(GLfloat* bufferArray, size_t sizeofBuffer, GLuint numVertices, std::shared_ptr<Shader>& shader) :
	m_BufferArray{ bufferArray }, 
	m_Shader{ shader }
{
	glGenVertexArrays(1, &VAO);
	VBO.reset(new GLuint, VBODeleter());
	glGenBuffers(1, VBO.get());

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	std::cout << "sizeof(bufferArray) = " << sizeofBuffer << std::endl;
	glBufferData(GL_ARRAY_BUFFER, sizeofBuffer, bufferArray, GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_AttribsUpdated = false;

	m_NumVertices = numVertices;
}

SimpleObject::SimpleObject(const SimpleObject& other) :
	VBO{ other.VBO }, 
	m_NumVertices{ other.m_NumVertices }, 
	m_Shader{ other.m_Shader }, 
	m_BufferArray{other.m_BufferArray}
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glBindVertexArray(0);

	m_AttribsUpdated = false;
	m_Attribs = other.m_Attribs;
}

SimpleObject::SimpleObject(const SimpleObject& other, std::shared_ptr<Shader>& shader) :
  VBO{ other.VBO },
  m_NumVertices{ other.m_NumVertices },
  m_Shader{ shader },
  m_BufferArray{ other.m_BufferArray }
{
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBindVertexArray(0);

  m_AttribsUpdated = false;
  m_Attribs = std::vector<Attrib>();
}

SimpleObject::~SimpleObject()
{
	glDeleteVertexArrays(1, &VAO);
	VBO.reset();
}

int SimpleObject::AddAttrib(GLint size, GLenum type, GLboolean normalized, GLint stride, GLint offset)
{
	m_Attribs.emplace_back(Attrib{ true, size, type, normalized, stride, (GLvoid*)offset });
	m_AttribsUpdated = false;
	return m_Attribs.size();
}

void SimpleObject::EnableAttrib(int location, bool enable)
{
	location--;
	if (location < m_Attribs.size() && location >= 0)
	{
		if (m_Attribs[location].enabled != enable)
		{
			m_Attribs[location].enabled = enable;
			m_AttribsUpdated = false;
		}
	}
	else
	{
		std::cout << "Location " << location + 1 << "Does not exist" << std::endl;
	}
}

void SimpleObject::UpdateAttribs()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	for (int i = 0; i < m_Attribs.size(); i++)
	{
		if (m_Attribs[i].enabled)
		{
			glVertexAttribPointer(i,
				m_Attribs[i].size,
				m_Attribs[i].type,
				m_Attribs[i].normalized,
				m_Attribs[i].stride,
				m_Attribs[i].offset);
			glEnableVertexAttribArray(i);
		}
		else
		{
			glDisableVertexAttribArray(i);
		}
	}
	glBindVertexArray(0);
}

void SimpleObject::Draw()
{
	if (!m_AttribsUpdated)
	{
		UpdateAttribs();
	}
	m_Shader->use();
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);

	glBindVertexArray(0);
}

const Shader& SimpleObject::GetShader()
{
	return *m_Shader.get();
}

void SimpleObject::SetShader(std::shared_ptr<Shader>& shader)
{
  m_Shader = shader;
}

const GLuint SimpleObject::GetVBO()
{
	return *VBO.get();
}

const void SimpleObject::BindVAO()
{
	glBindVertexArray(VAO);
}

const void SimpleObject::UnbindVAO()
{
	glBindVertexArray(0);
}
>>>>>>> 39a59811202a2b96f008b4abe322d1c65539dc6c:LearnOpenGL/SimpleObject.cpp
