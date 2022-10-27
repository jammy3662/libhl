#pragma once

#include <stb/stb_image.h>

struct Image
{
	u8* data; // pixel data
	int width, height, depth;
	int channels; // number of fields per pixel
	uint type;
};

// load 2D image from a file
Image createImage(const char* filename)
{
	Image ret;
	
	ret.type = GL_TEXTURE_2D;
	ret.depth = 0;
	ret.data = stbi_load(filename, &ret.width, &ret.height, &ret.channels, 0);
	
	if (!ret.data)
	{
		fprintf(stderr, "[Texture] Failed to load from file '%s'!\n", filename);
	}
	
	return ret;
}

// create an empty 2D or 3D image
// optionally zeroes out buffer
Image createImage(int width, int height, int depth, int channels, int zeroData = false)
{
	Image ret;
	
	ret.width = width;
	ret.height = height;
	ret.depth = depth;
	ret.channels = channels;
	
	if (depth < 1) ret.type = GL_TEXTURE_2D;
	else ret.type = GL_TEXTURE_3D;
	
	u64 pixelNum = width * height * depth;
	u64 dataSize = pixelNum * channels * sizeof(uint8);
	
	ret.data = (u8*) malloc(dataSize);
	if (zeroData)
		memset(ret.data, 0, dataSize);
}

void unloadImage(Image img)
{
	free(img.data);
}

// ================================

#define TEXTURE_SHADER_VS "\
#version 330\
\n	\
\n	layout (location = 0) in vec3 vertexPos;\
\n	layout (location = 1) in vec2 vertexCoord;\
\n	\
\n out vec2 fragCoord;\
\n \
\n uniform mat4 transform;\
\n	\
\n	void main()\
\n	{\
\n 	// Calculate final vertex position\
\n	   gl_Position = transform * vec4(vertexPos, 1.0);\
\n \
\n	   // Send vertex attributes to fragment shader\
\n		fragCoord = vertexCoord;\
\n	}\
"
#define TEXTURE_SHADER_FS "\
#version 330\
\n	\
\n in vec2 fragCoord;\
\n \
\n layout (location = 0) out vec4 finalColor;\
\n	\
\n	uniform sampler2D texture;\
\n uniform vec4 diffuse;\
\n	\
\n	void main()\
\n	{\
\n		finalColor = diffuse * texture2D(texture, fragCoord);\
\n	}\
"

struct Texture
{
	uint id; // opengl resource id
	uint format;
	uint type;
	int slot; // which texture slot currently bound to
};

int textureSlot = 0;

uint hl_textureQuad;
Texture hl_blankTexture;

Texture createTexture(void* _image)
{
	Image image = *(Image*)_image;
	Texture tex;
	
	if (image.channels == 1) tex.format = GL_RED;
	else if (image.channels == 2) tex.format = GL_RG;
	else if (image.channels == 3) tex.format = GL_RGB;
	else if (image.channels == 4) tex.format = GL_RGBA;
	else
	{
		fprintf(stderr, "[Texture] Unsupported channel count (%i)\n", image.channels);
		return;
	}
	
	glGenTextures(1, &tex.id);
	glBindTexture(image.type, tex.id);
	if (image.type == GL_TEXTURE_2D)
	{
		tex.type = GL_TEXTURE_2D;
		glTexImage2D(image.type, 0, tex.format, image.width, image.height, 0, tex.format, GL_UNSIGNED_BYTE, image.data);
		glGenerateMipmap(image.type);
	}
	else if (image.type == GL_TEXTURE_3D)
	{
		tex.type = GL_TEXTURE_3D;
		glTexImage3D(image.type, 0, tex.format, image.width, image.height, image.depth, 0, tex.format, GL_UNSIGNED_BYTE, image.data);
	}
	else
	{
		fprintf(stderr, "[Texture] Image tex.format unsupported for texture load (neither 2D nor 3D)\n");
	}
	
	return tex;
}

void activateTexture(Texture& tex)
{
	tex.slot = textureSlot;
	glActiveTexture(GL_TEXTURE0 + tex.slot);
	textureSlot++;
	
	glBindTexture(tex.type, tex.id);
}

void clearTextures()
{
	textureSlot = 0;
	glActiveTexture(GL_TEXTURE0);
}

void drawTexture(Texture texture, int x, int y, int width, int height, Color color);
inline
void drawRect(int x, int y, int width, int height, Color color)
{ drawTexture(hl_blankTexture, x, y, width, height, color); }

void setupTextures()
{
	uint buffer;
	glGenBuffers(1, &hl_textureQuad);
	
	glGenVertexArrays(1, &hl_textureQuad);
	glGenBuffers(1, &buffer);

	glBindVertexArray(hl_textureQuad);
	
	typedef struct {float x, y, z, u, v;} Vert;
	
	Vert data[] =
	{
		{-1.0,  1.0, 1.0,  0.0, 1.0},
		{-1.0, -1.0, 1.0,  0.0, 0.0},
		{ 1.0, -1.0, 1.0,  1.0, 0.0},
		 
		{-1.0,  1.0, 1.0,  0.0, 1.0},
		{ 1.0, -1.0, 1.0,  1.0, 0.0},
		{ 1.0,  1.0, 1.0,  1.0, 1.0},
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert),
		(void*) offsetof(Vert, x));
	glEnableVertexAttribArray(0);
		
	// uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert),
		(void*) offsetof(Vert, u));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	
	hl_blankTexture = createTexture(&blankImg);
}