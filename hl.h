#pragma once

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

#define uint unsigned int

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long

#define i8 char
#define i16 short
#define i32 int
#define i64 long

#include <ext.h>

#include "core.h"
#include "texture.h"
#include "frame.h"
#include "shader.h"
#include "mesh.h"

//
// CORE
//
 
void setup()
{
	setupTextures();
	hl_textureShader = createShader(TEXTURE_SHADER_VS, TEXTURE_SHADER_FS);
	hl_textureShader.setName("hlTextureShader");
}

//
// TEXTURE
//

void drawTexture(Texture texture, int x, int y, int width, int height, Color color)
{
	useShader(&hl_textureShader);
	
	vec4 glm_color(color.r, color.g, color.b, color.a);
	hl_textureShader.setVec4("diffuse", glm_color);
	hl_textureShader.setTexture("texture", texture);
	
	//
	// Calculate offset and scaling
	// for the specified
	// destination rectangle
	//
	
	float w = (currentFrame) ? currentFrame->width : hl.fwidth;
	float h = (currentFrame) ? currentFrame->height : hl.fheight;
	// if drawing to a framebuffer, use its dimensions
	// otherwise, use the viewport dimensions
	
	mat4 transform = identity<mat4>();
	transform *= translate(vec3((float)(2*x/w - 1), (float)(2*y/h - 1), 0.0));
	transform *= scale(vec3((float)width/w, (float)height/h, 1.0));
	
	hl_textureShader.setMat4("transform", transform);
	
	glBindVertexArray(hl_textureQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}