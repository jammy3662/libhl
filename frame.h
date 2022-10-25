#pragma once

struct Frame:
public Texture
{
	uint fbo;
	uint color;
	uint depth;
	
	int ops;
	
	uint minFilter;
	uint magFilter;
};

void enableFrame(Frame frame)
{
	glBindFramebuffer(frame.ops, frame.fbo);
	glEnable(GL_DEPTH_TEST);
}

void defaultFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}

// stencil buffer does not work yet!
Frame createFrame(int write, int read, int width, int height, int color = 1, int depth = 1, int stencil = 0, uint magFilter = GL_LINEAR, uint minFilter = GL_LINEAR)
{
	Frame ret;
	ret.color = 0;
	ret.depth = 0;
	
	ret.minFilter = minFilter;
	ret.magFilter = magFilter;
	
	glGenFramebuffers(1, &ret.fbo);
	
	if (color)
	{
		glGenTextures(1, &ret.color);
		glBindTexture(GL_TEXTURE_2D, ret.color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	}
	
	if (depth)
	{
		glGenTextures(1, &ret.depth);
		glBindTexture(GL_TEXTURE_2D, ret.depth);
		
		if (stencil)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_BYTE, 0);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		}
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	}
	
	ret.ops = GL_DRAW_FRAMEBUFFER;
	if (read && write) ret.ops = GL_FRAMEBUFFER;
	else if (read) ret.ops = GL_READ_FRAMEBUFFER;
	 
	glBindFramebuffer(ret.ops, ret.fbo);
	if (color)
	{
		glFramebufferTexture2D(ret.ops, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ret.color, 0);
	}
	if (depth)
	{
		if (stencil)
		{
			glFramebufferTexture2D(ret.ops, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ret.depth, 0);
		}
		else
		{
			glFramebufferTexture2D(ret.ops, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ret.depth, 0);
		}
	}
	
	fprintf(stderr, "Framebuffer creation status: %X\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	return ret;
}

void clearFrame(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void presentFrame()
{
	glfwPollEvents();
	glfwSwapBuffers(hl.window);
}