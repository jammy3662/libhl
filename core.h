#pragma once

typedef struct {float r, g, b, a;} Color;

struct
{
	// glfw window handle
	GLFWwindow* window;
	
	// frame time calculation
	float lastFrameTime;
	float thisFrameTime;
	float delta;
	
	// frame throttle
	float accumulator;
	float frameTime;
	
	// frame (internal render target / backbuffer)
	uint fwidth = 1920;
	uint fheight = 1080;

	// window (onscreen drawing)
	uint wwidth = 1280;
	uint wheight = 720;
}
hl;

inline
void setFrame(uint width, uint height)
{
	hl.fwidth = width;
	hl.fheight = height;
}

inline
void setWindow(uint width, uint height)
{
	hl.wwidth = width;
	hl.wheight = height;
}

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

int shouldRender()
{
	calculateDelta();
	
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

uint openWindow(char* title)
{
	hl.window = glfwCreateWindow(hl.wwidth, hl.wheight, title, 0, 0);
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
	
	glViewport(0,0, hl.fwidth, hl.fheight);
	
	return 0;
}