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

const std::string root_dir(logl_root);

GLuint constTextureID, inTextureID, outTextureID;

std::shared_ptr<Shader> shaderProgram;
std::shared_ptr<Shader> computeProgram;

SimpleObject* container;

void checkErrors(const std::string& location)
{
	GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error in \"" << location << "\": " << glewGetErrorString(e) << " (" << e << ")" << std::endl;
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

	// Build and Compile out shader program
	shaderProgram.reset(new Shader(root_dir+"/Shaders/TexturedVertex.vert", root_dir+"/Shaders/TexturedFragment.frag"));
	checkErrors("create shader program");
	computeProgram.reset(new Shader(root_dir + "/Shaders/GPUHeatTransfer.comp"));
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

	container = new SimpleObject(&vertices[0], sizeof(vertices), 6, shaderProgram);
	checkErrors("create container simple object");
	container->AddAttrib(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	container->AddAttrib(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

	// load textures
	int width, height;
	unsigned char *image;
	image = SOIL_load_image((root_dir+"/Textures/HeatSource.png").c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	if (width != screenWidth || height != screenHeight)
	{
		std::cout << "Screen size doesn't match texture size" << std::endl;
		return 0;
	}

	float* pixels = new float[screenWidth * screenHeight];

	for (int i = 0; i < screenWidth * screenHeight; i++)
	{
		pixels[i] = (static_cast<float>(image[i*3])/255.0f);
	}
	SOIL_free_image_data(image);

	glGenTextures(1, &constTextureID);
	checkErrors("genTextures");
	glActiveTexture(GL_TEXTURE0);
	checkErrors("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, constTextureID);
	checkErrors("glBindTexture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, pixels);
	delete pixels;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	checkErrors("generate consttexture");
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, screenWidth, screenHeight);

	glGenTextures(1, &inTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inTextureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, screenWidth, screenHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	checkErrors("generate intexture");

	glGenTextures(1, &outTextureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, outTextureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, screenWidth, screenHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE0);
	
	checkErrors("generate outtexture");

	computeProgram->use();
	glUniform1i(computeProgram->getUniformLocation("constImage"), 0);
	glUniform1i(computeProgram->getUniformLocation("inputImage"), 1);
	glUniform1i(computeProgram->getUniformLocation("outputImage"), 2);
	checkErrors("set destTex uniform");
	shaderProgram->use();
	glUniform1i(shaderProgram->getUniformLocation("ourTexture"), 2);
	checkErrors("Set ourTexture uniform");

	// Set texture units
	/*shaderProgram->use();
	glUniform1i(shaderProgram->getUniformLocation("material.diffuse"), 0);*/
	long frameCount = 0;
	GLfloat PreviousTime = 0;
	bool isOutTexture = false;
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
		glBindImageTexture(0, constTextureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
		if (isOutTexture)
		{
			glBindImageTexture(1, inTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
			glBindImageTexture(2, outTextureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
			glUniform1i(computeProgram->getUniformLocation("inputImage"), 2);
			glUniform1i(computeProgram->getUniformLocation("outputImage"), 1);
		}
		else
		{
			glBindImageTexture(1, inTextureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
			glBindImageTexture(2, outTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
			glUniform1i(computeProgram->getUniformLocation("inputImage"), 1);
			glUniform1i(computeProgram->getUniformLocation("outputImage"), 2);
		}
		
		checkErrors("BindImageTexture");

		glDispatchCompute(screenWidth / 16, screenHeight / 16, 1);
		checkErrors("Dispatch Compute Shader");
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


		shaderProgram->use();
		if (isOutTexture)
		{
			glUniform1i(shaderProgram->getUniformLocation("ourTexture"), 1);
		}
		else
		{
			glUniform1i(shaderProgram->getUniformLocation("ourTexture"), 2);
		}
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, constTextureID);
		checkErrors("Bind Texture");
		container->Draw();
		checkErrors("Draw To Screen");

		isOutTexture = !isOutTexture;

		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// properly de-allocate all resources once they've outlived their purpose
	glfwTerminate();

	delete container;

	return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement(float dt)
{
	if (keys[GLFW_KEY_R])
	{
		int width, height;
		unsigned char *image;
		image = SOIL_load_image((root_dir + "/Textures/HeatSource.png").c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		if (width != screenWidth || height != screenHeight)
		{
			std::cout << "Screen size doesn't match texture size" << std::endl;
			exit(20);
		}

		float* pixels = new float[screenWidth * screenHeight];

		for (int i = 0; i < screenWidth * screenHeight; i++)
		{
			pixels[i] = (static_cast<float>(image[i * 3]) / 255.0f);
		}
		SOIL_free_image_data(image);

		glActiveTexture(GL_TEXTURE0);
		checkErrors("glActiveTexture");
		glBindTexture(GL_TEXTURE_2D, constTextureID);
		checkErrors("glBindTexture");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, pixels);
		delete pixels;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		keys[GLFW_KEY_R] = false;
	}

	if (keys[GLFW_KEY_S])
	{
		shaderProgram.reset(new Shader(root_dir + "/Shaders/TexturedVertex.vert", root_dir + "/Shaders/TexturedFragment.frag"));
		container->SetShader(shaderProgram);
		keys[GLFW_KEY_S] = false;
	}

	if (keys[GLFW_KEY_C])
	{
		computeProgram.reset(new Shader(root_dir + "/Shader/GPUHeatTransfer.comp"));
		keys[GLFW_KEY_C] = false;
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
