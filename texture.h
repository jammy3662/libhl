#pragma once

#include <stb/stb_image.h>

struct Image
{
	uint8* data; // pixel data
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
	stbi_image_free(img.data);
}

// ================================

struct Texture
{
	uint id; // opengl resource id
	int format;
	
	Image img;
};

inline
Texture createTexture(Image image)
{
	Texture ret;
	ret.img = image;
	return ret;
}

void uploadTexture(Texture& tex)
{
	if (tex.img.channels == 1) tex.format = GL_R;
	else if (tex.img.channels == 3) tex.format = GL_RGB;
	else if (tex.img.channels == 4) tex.format = GL_RGBA;
	else
	{
		fprintf(stderr, "[Texture] Unsupported channel count (%i)\n", tex.img.channels);
		return;
	}
	
	glGenTextures(1, &tex.id);
	glBindTexture(tex.img.type, tex.id);
	if (tex.img.type == GL_TEXTURE_2D)
	{
		glTexImage2D(tex.img.type, 0, tex.format, tex.img.width, tex.img.height, 0, tex.format, GL_UNSIGNED_BYTE, tex.img.data);
		glGenerateMipmap(tex.img.type);
	}
	else if (tex.img.type == GL_TEXTURE_3D)
	{
		glTexImage3D(tex.img.type, 0, tex.format, tex.img.width, tex.img.height, tex.img.depth, 0, tex.format, GL_UNSIGNED_BYTE, tex.img.data);
	}
	else
	{
		fprintf(stderr, "[Texture] Image tex.format unsupported for texture load (neither 2D nor 3D)\n");
	}
}

void resetTextures()
{
	glActiveTexture(GL_TEXTURE0);
}