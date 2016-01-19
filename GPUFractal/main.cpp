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
#include "root_directory.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>

// Properties
const GLuint screenWidth = 1024, screenHeight = 1024;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void error_callback(int error, const char* message);
void Do_Movement(float dt);

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = screenWidth / 2.0f;
GLfloat lastY = screenHeight / 2.0f;
bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

const int Major = 4;
const int Minor = 5;

float ab[2] = { 0, 0 };
float offset[2] = { 0, 0 };
float scale = 2;
unsigned int loops = 500;

void checkErrors(const std::string& location)
{
	GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error in \"" << location << "\": " << gluErrorString(e) << " (" << e << ")" << std::endl;
		exit(20);
	}
}

int main(int argc, char** argv)
{
	glfwSetErrorCallback(error_callback);
	// Init GLFW
	if (!glfwInit())
	{
		std::cout << "glfw init error" << std::endl;
	}
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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	glGetError();

	// Define the viewport dimentions
	glViewport(0, 0, screenWidth, screenHeight);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	checkErrors("Enable Depth Test");

	std::string root_dir(logl_root);

	// Build and Compile out shader program
	std::shared_ptr<Shader> shaderProgram{ new Shader(root_dir+"/Shaders/TexturedVertex.vert", root_dir+"/Shaders/TexturedFragment.frag") };
	checkErrors("create shader program");
	std::shared_ptr<Shader> computeProgram{ new Shader(root_dir + "/Shaders/GPUFract.comp") };
	checkErrors("create compute program");

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions        // Texture Coords
		 1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // Top Right
		 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  // Bottom Right
		-1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  // Bottom Left
		-1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  // Bottom left
		-1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  // Top Left
		 1.0f,  1.0f,  0.0f, 1.0f, 1.0f   // Top Right
	};

	SimpleObject container(&vertices[0], sizeof(vertices), 6, shaderProgram);
	checkErrors("create container simple object");
	container.AddAttrib(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	container.AddAttrib(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

	// load textures
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, screenWidth, screenHeight);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	checkErrors("generate texture");
	computeProgram->use();
	glUniform1i(computeProgram->getUniformLocation("destTex"), 0);
	checkErrors("set destTex uniform");
	shaderProgram->use();
	glUniform1i(shaderProgram->getUniformLocation("ourTexture"), 0);
	checkErrors("Set ourTexture uniform");

	// Set texture units
	/*shaderProgram->use();
	glUniform1i(shaderProgram->getUniformLocation("material.diffuse"), 0);*/
	long frameCount = 0;
	GLfloat PreviousTime = 0;
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		frameCount++;
		GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (lastFrame - PreviousTime > 1.0f)
		{
			char fps[256];

			sprintf(fps, "%3.1f fps", frameCount / (currentFrame - PreviousTime));

			PreviousTime = currentFrame;

			glfwSetWindowTitle(window, fps);

			frameCount = 0;
		}

		// Check and call events
		glfwPollEvents();
		Do_Movement(deltaTime);

		// clear the color and depth buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeProgram->use();

		glUniform2f(computeProgram->getUniformLocation("ab"), ab[0], ab[1]);
		glUniform2f(computeProgram->getUniformLocation("offset"), offset[0], offset[1]);
		glUniform1f(computeProgram->getUniformLocation("scale"), scale);
		glUniform1ui(computeProgram->getUniformLocation("maxLoops"), loops);
		glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		checkErrors("BindImageTexture");

		glDispatchCompute(screenWidth / 16, screenHeight / 16, 1);
		checkErrors("Dispatch Compute Shader");
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


		shaderProgram->use();
		glBindTexture(GL_TEXTURE_2D, textureID);
		checkErrors("Bind Texture");
		container.Draw();
		checkErrors("Draw To Screen");

		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// properly de-allocate all resources once they've outlived their purpose
	glfwTerminate();

	return 0;
}

void reset()
{
	scale = 2;
	offset[0] = 0;
	offset[1] = 0;
	ab[0] = -0.747753;
	ab[1] = 0.05f;
}

// Moves/alters the camera positions based on user input
void Do_Movement(float dt)
{
	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		ab[0] += 0.1f * dt;
	}
	if (keys[GLFW_KEY_S])
	{
		ab[0] -= 0.1f * dt;
	}
	if (keys[GLFW_KEY_A])
	{
		ab[1] -= 0.1f * dt;
	}
	if (keys[GLFW_KEY_D])
	{
		ab[1] += 0.1f * dt;
	}
	if (keys[GLFW_KEY_Q])
	{
		scale -= 0.5f * scale * dt;
	}
	if (keys[GLFW_KEY_E])
	{
		scale += 0.5f * scale * dt;
	}
	if (keys[GLFW_KEY_R])
	{
		reset();
		keys[GLFW_KEY_R] = false;
	}
	if (keys[GLFW_KEY_UP])
	{
		offset[0] += 0.1f * scale * dt;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		offset[0] -= 0.1f * scale * dt;
	}
	if (keys[GLFW_KEY_LEFT])
	{
		offset[1] -= 0.1f * scale * dt;
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		offset[1] += 0.1f * scale * dt;
	}
	if (keys[GLFW_KEY_PAGE_UP])
	{
		loops += 100;
		keys[GLFW_KEY_PAGE_UP] = false;
	}
	if (keys[GLFW_KEY_PAGE_DOWN])
	{
		loops -= 100;
		keys[GLFW_KEY_PAGE_DOWN] = false;
	}
	if (keys[GLFW_KEY_SPACE])
	{
		std::cout << "===============================" << std::endl;
		std::cout << "\na = " << ab[0] << ", b = " << ab[1] << std::endl;
		std::cout << "scale = " << scale << std::endl;
		std::cout << "offset X = " << offset[0] << " \t offset Y = " << offset[1] << std::endl;
		std::cout << "max iterations = " << loops << std::endl;
		std::cout << "===============================" << std::endl;
		keys[GLFW_KEY_SPACE] = false;
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
