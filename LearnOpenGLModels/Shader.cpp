#include <Shader.h>

bool CheckShaderCompilation(GLuint shader, const GLchar* filename = nullptr)
{
  GLint success = 1;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    if (filename != nullptr)
    {
      std::cout << filename << std::endl;
    }
    std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  return success != 0;
}

bool CheckShaderLink(GLuint program)
{
  GLint success = 1;
  GLchar infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
  }

  return success != 0;
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
  std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handles
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

  const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;

	// VertexShader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

  CheckShaderCompilation(vertex, vertexPath);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	CheckShaderCompilation(fragment, fragmentPath);

	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);

	CheckShaderLink(this->Program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) :
	Shader(vertexPath.c_str(), fragmentPath.c_str())
{

}

void Shader::use()
{ 
  glUseProgram(this->Program); 
}

GLuint Shader::getUniformLocation(const GLchar* name) const
{ 
	GLint location = glGetUniformLocation(this->Program, name);
	return location;
}


