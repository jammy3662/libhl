#pragma once

void clearFrame(float r, float g, float b, float a,
	int flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
{
	glClearColor(r, g, b, a);
	glClear(flags);
}

void presentFrame()
{
	glfwSwapBuffers(hl.window);
	glfwPollEvents();
}

struct Buffer
{
	uint fbo;
	uint texture;
	
	void init()
	{
		
	}
};