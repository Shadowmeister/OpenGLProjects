// Std. includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "SimpleObject.h"
#include "Texture.h"
#include "LearnOpenGLConfig.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>

// Properties
const GLuint screenWidth = 1920, screenHeight = 1080;

struct Material {
	std::unique_ptr<Texture> diffuseTexture;
	std::unique_ptr<Texture> specularTexture;
	std::unique_ptr<Texture> emissiveTexture;
	GLfloat shininess;
};

struct DirLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight {
	glm::vec3 position;

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void error_callback(int error, const char* message);
void Do_Movement();

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = screenWidth / 2.0f;
GLfloat lastY = screenHeight / 2.0f;
bool keys[1024];

Material objectMaterial{ nullptr, nullptr, nullptr, 8.0f };
DirLight dirLight{glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f)};
PointLight lightProperties[]{ 
		PointLight{ glm::vec3(0.7f, 0.2f, 2.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) },
		PointLight{ glm::vec3(2.3f, -3.3f, -4.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) },
		PointLight{ glm::vec3(-4.0f, 2.0f, -12.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) },
		PointLight{ glm::vec3(0.0f, 0.0f, -3.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) }
};

// Positions all containers
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

const int Major = 3;
const int Minor = 3;

int main(int argc, char** argv)
{
  std::cout << argv[0] << " Version " << LearnOpenGL_VERSION_MAJOR << "." << LearnOpenGL_VERSION_MINOR << std::endl;
	glfwSetErrorCallback(error_callback);
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Error creating with with OpenGL Version " << Major << "." << Minor << std::endl;
	}

	glfwMakeContextCurrent(window);

	// Set required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimentions
	glViewport(0, 0, screenWidth, screenHeight);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Build and Compile out shader program
	std::shared_ptr<Shader> shaderProgram { new Shader("Shaders/BasicVertex.vert", "Shaders/LitObjectFragment.frag") };
	std::shared_ptr<Shader> lampProgram{ new Shader("Shaders/BasicVertex.vert", "Shaders/LampFragment.frag") };

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions          // Normals        // Texture Coords
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	SimpleObject container(&vertices[0], sizeof(vertices), 36, shaderProgram);
	container.AddAttrib(3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	container.AddAttrib(3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
	container.AddAttrib(2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat));
	SimpleObject light(container, lampProgram);
	light.AddAttrib(3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	// load textures
	objectMaterial.diffuseTexture = std::unique_ptr < Texture > {new Texture("Textures/container2.png", Texture::DIFFUSE, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST)};
	objectMaterial.specularTexture = std::unique_ptr < Texture > {new Texture("Textures/container2_specular.png", Texture::SPECULAR, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST)};
	objectMaterial.emissiveTexture = std::unique_ptr < Texture > {new Texture("Textures/matrix.jpg", Texture::EMISSIVE, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST)};

	// Set texture units
	shaderProgram->use();
	glUniform1i(shaderProgram->getUniformLocation("material.diffuse"), 0);
	glUniform1i(shaderProgram->getUniformLocation("material.specular"), 1);
	glUniform1i(shaderProgram->getUniformLocation("material.emissive"), 2);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// clear the color and depth buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//lightProperties.position.x = sin(currentFrame);
		//lightProperties.position.z = cos(currentFrame);

		// Use cooresponding shader when setting uniforms/drawing objects
		shaderProgram->use();
		glUniform3f(shaderProgram->getUniformLocation("viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		// set material properties
		glUniform1f(shaderProgram->getUniformLocation("material.shininess"), objectMaterial.shininess);

		glUniform3f(shaderProgram->getUniformLocation("dirLight.direction"), dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);
		glUniform3f(shaderProgram->getUniformLocation("dirLight.ambient"), dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z);
		glUniform3f(shaderProgram->getUniformLocation("dirLight.diffuse"), dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z);
		glUniform3f(shaderProgram->getUniformLocation("dirLight.specular"), dirLight.specular.x, dirLight.specular.y, dirLight.specular.z);

		for (int i = 0; i < 4; i++)
		{
			std::string num = std::to_string(i);

			glUniform3f(shaderProgram->getUniformLocation(("pointLights[" + num + "].position").c_str()), lightProperties[i].position.x, lightProperties[i].position.y, lightProperties[i].position.z);
			glUniform3f(shaderProgram->getUniformLocation(("pointLights[" + num + "].ambient").c_str()), lightProperties[i].ambient.x, lightProperties[i].ambient.y, lightProperties[i].ambient.z);
			glUniform3f(shaderProgram->getUniformLocation(("pointLights[" + num + "].diffuse").c_str()), lightProperties[i].diffuse.x, lightProperties[i].diffuse.y, lightProperties[i].diffuse.z);
			glUniform3f(shaderProgram->getUniformLocation(("pointLights[" + num + "].specular").c_str()), lightProperties[i].specular.x, lightProperties[i].specular.y, lightProperties[i].specular.z);
			glUniform1f(shaderProgram->getUniformLocation(("pointLights[" + num + "].constant").c_str()), lightProperties[i].constant);
			glUniform1f(shaderProgram->getUniformLocation(("pointLights[" + num + "].linear").c_str()), lightProperties[i].linear);
			glUniform1f(shaderProgram->getUniformLocation(("pointLights[" + num + "].quadratic").c_str()), lightProperties[i].quadratic);
		}

		// Create camera transformations
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		// Pass matrices to the shader
		glUniformMatrix4fv(shaderProgram->getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shaderProgram->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		objectMaterial.diffuseTexture->Bind();
		// Bind specular map
		glActiveTexture(GL_TEXTURE1);
		objectMaterial.specularTexture->Bind();
		// Bind emissive map
		glActiveTexture(GL_TEXTURE2);
		objectMaterial.emissiveTexture->Bind();
		
		// Draw the container (using container's vertex attributes)
		glm::mat4 model;
		for (GLuint i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(shaderProgram->getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
			container.Draw();
		}

		// Also draw the lamp object, again binding the appropriate shader
		lampProgram->use();
		// Set location matrices on the lamp shader (these could be different on a different shader)
		glUniformMatrix4fv(lampProgram->getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(lampProgram->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the light object (using light's vertex attributes)
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, lightProperties[i].position);
			model = glm::scale(model, glm::vec3(0.2f)); // Makeit a smaller cube
			glUniformMatrix4fv(lampProgram->getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(lampProgram->getUniformLocation("lampColor"), lightProperties[i].diffuse.x, lightProperties[i].diffuse.y, lightProperties[i].diffuse.z);
			light.Draw();
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// properly de-allocate all resources once they've outlived their purpose
	glfwTerminate();

	return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = static_cast<GLfloat>(xpos);
		lastY = static_cast<GLfloat>(ypos);
		firstMouse = false;
	}

	GLfloat xoffset = static_cast<GLfloat>(xpos - lastX);
	GLfloat yoffset = static_cast<GLfloat>(lastY - ypos); // Reversed since y-coordinates go from bottom to top

	lastX = static_cast<GLfloat>(xpos);
	lastY = static_cast<GLfloat>(ypos);

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<GLfloat>(yoffset));
}

void error_callback(int error, const char* message)
{
	std::cout << "Error " << error << std::endl << "\t Message: " << message << std::endl;
}
