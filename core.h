#pragma once

typedef struct {float r, g, b, a;} Color;

struct
{
	GLFWwindow* window;
	float lastFrameTime;
	float thisFrameTime;
	float delta;
	float accumulator;
	float frameTime;
}
hl;

void setFramerate(uint framerate)
{
	hl.frameTime = 1.0 / (float)framerate;
}

void calculateDelta()
{
	hl.thisFrameTime = glfwGetTime();
	hl.delta = hl.thisFrameTime - hl.lastFrameTime;
	hl.accumulator += hl.delta;
	
	hl.lastFrameTime = hl.thisFrameTime;
}

uint frameStep()
{
	if (hl.accumulator >= hl.frameTime)
	{
		hl.accumulator = 0;
		return 1;
	}
	return 0;
}

void init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	hl.lastFrameTime = glfwGetTime();
	hl.thisFrameTime = glfwGetTime();
	hl.delta = 0;
	hl.accumulator = 0;
}

void deinit()
{
	glfwTerminate();
}

uint openWindow(int width, int height, int viewportWidth, int viewportHeight, char* title)
{
	hl.window = glfwCreateWindow(width, height, title, 0, 0);
	if (!hl.window)
	{
		fprintf(stderr, "Failed to create window\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(hl.window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return 1;
	}
	
	glViewport(0,0, viewportWidth, viewportHeight);
	
	return 0;
}