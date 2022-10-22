#pragma once

#include <stb/stb_image.h>

struct Image
{
	uint8* data; // pixel data
	int width, height, depth;
	int channels; // number of fields per pixel
	uint type;
	
	void load(Image img)
	{
		*this = img;
	}
	
	// create an empty 2D or 3D image
	// optionally zeroes out buffer
	void create(int _width, int _height, int _depth, int _channels, int zeroData = false)
	{		
		width = _width;
		height = _height;
		depth = _depth;
		channels = _channels;
		
		if (depth < 1) type = GL_TEXTURE_2D;
		else type = GL_TEXTURE_3D;
		
		uint64 pixelNum = width * height * depth;
		uint64 dataSize = pixelNum * channels * sizeof(uint8);
		
		data = (uint8*) malloc(dataSize);
		if (zeroData)
			memset(data, 0, dataSize);
	}
	
	// load 2D image from a file
	void loadFile(const char* filename)
	{
		type = GL_TEXTURE_2D;
		depth = 0;
		data = stbi_load(filename, &width, &height, &channels, 0);
	}
};

struct Texture
{
	uint id; // opengl resource id
	int format;
	
	Image img;
	
	void loadImage(Image image)
	{
		img.load(image);
	}
	
	void unloadImage()
	{
		stbi_image_free(img.data);
	}
	
	void load()
	{
		if (img.channels == 1) format = GL_R;
		else if (img.channels == 3) format = GL_RGB;
		else if (img.channels == 4) format = GL_RGBA;
		else
		{
			fprintf(stderr, "[Texture] Unsupported channel count (%i)\n", img.channels);
			return;
		}
		
		glGenTextures(1, &id);
		glBindTexture(img.type, id);
		if (img.type == GL_TEXTURE_2D)
		{
			glTexImage2D(img.type, 0, format, img.width, img.height, 0, format, GL_UNSIGNED_BYTE, img.data);
			glGenerateMipmap(img.type);
		}
		else if (img.type == GL_TEXTURE_3D)
		{
			glTexImage3D(img.type, 0, format, img.width, img.height, img.depth, 0, format, GL_UNSIGNED_BYTE, img.data);
		}
		else
		{
			fprintf(stderr, "[Texture] Image format unsupported for texture load (neither 2D nor 3D)\n");
		}
	}
};

void resetTexture()
{
	glActiveTexture(GL_TEXTURE0);
}