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
#include "Model.h"
#include "LearnOpenGLConfig.h"
#include "root_directory.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>

// Properties
const GLuint screenWidth = 1920, screenHeight = 1080;

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
	glEnable(GL_FRAMEBUFFER_SRGB);

	std::string root_dir(logl_root);

	// Build and Compile out shader program
	std::shared_ptr<Shader> shaderProgram{ new Shader(root_dir+"/Shaders/model_loading.vert", root_dir+"/Shaders/model_loading.frag") };

	Model ourModel(root_dir + "/Models/nanosuit/nanosuit.obj");

	// Draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::vec3 pointLightPositions[] = {
		glm::vec3(2.3f, -1.6f, -3.0f),
		glm::vec3(0.0f, 0.9f, 1.0f)
	};

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
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram->use();
		// Transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(shaderProgram->getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(shaderProgram->getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));

		// Set the lighting uniforms
		glUniform3f(shaderProgram->getUniformLocation("viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		pointLightPositions[1].x = sin(glfwGetTime()) * 2;
		pointLightPositions[1].z = cos(glfwGetTime()) * 2;

		pointLightPositions[0].x = -pointLightPositions[1].x;
		pointLightPositions[0].z = pointLightPositions[1].z;

		glUniform3f(shaderProgram->getUniformLocation("lightPos[0]"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(shaderProgram->getUniformLocation("pointLights[0].ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(shaderProgram->getUniformLocation("pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(shaderProgram->getUniformLocation("pointLights[0].specular"), 0.2f, 0.2f, 0.2f);
		glUniform1f(shaderProgram->getUniformLocation("pointLights[0].constant"), 1.0f);
		glUniform1f(shaderProgram->getUniformLocation("pointLights[0].linear"), 0.009);
		glUniform1f(shaderProgram->getUniformLocation("pointLights[0].quadratic"), 0.0032);

		glUniform3f(shaderProgram->getUniformLocation("lightPos[1]"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(shaderProgram->getUniformLocation("pointLights[1].ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(shaderProgram->getUniformLocation("pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(shaderProgram->getUniformLocation("pointLights[1].specular"),0.2f, 0.2f, 0.2f);
		glUniform1f(shaderProgram->getUniformLocation("pointLights[1].constant"), 1.0f);
		glUniform1f(shaderProgram->getUniformLocation("pointLights[1].linear"), 0.009);
		glUniform1f(shaderProgram->getUniformLocation("pointLights[1].quadratic"), 0.0032);

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // it's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(shaderProgram->getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
		ourModel.Draw(*shaderProgram);

		// Swap the buffers
		glfwSwapBuffers(window);
	}
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
