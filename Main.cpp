#include "Main.h"

const int screenWidth = 1920;
const int screenHeight = 1080;

Camera cam;

// Called when the user resizes the window & resizes viewport correctly
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

float getRandomFloat(float low, float high);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	TextureLoader::initialize();
	ChunkLoader::initialize();
	for(int i = -2; i < 3; i++)
		for(int j = -2; j < 3; j++)
			ChunkLoader::forceLoadChunk(i, j);

	glViewport(0, 0, screenWidth, screenHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSetCursorPosCallback(window, mouse_callback);

	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		
		glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ChunkLoader::tickLoadChunks(cam);
		cam.updateFrustrum();
		ChunkLoader::drawChunks(cam);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

float getRandomFloat(float low, float high)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = high - low;
	float r = random * diff;
	return low + r;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int ksdg = 1;
bool last = false;

void processInput(GLFWwindow* window)
{
	TextureLoader::getCubeShader().setMat4("view", cam.getView());

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	/*if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		last = true;
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
	{
		if(last)
			ChunkLoader::unloadChunk(ksdg--, 0);
		last = false;
	}*/
	
	cam.processInput(window);
	Time::processInput();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	cam.mouse_callback(window, xpos, ypos);
}