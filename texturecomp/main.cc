#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stb/stb_image.h>

#include <ext.h>

#define PATH_DELIM '/'
#ifdef _WIN32
	#define PATH_DELIM '\'
#endif

#define USAGE "\n\
Usage:\n\
tcomp [HEADER_PATH] [IN_FILE1] [IN_FILE2] ...\n\
HEADER_PATH is a path to the header file to send output.\n\
This will overwrite any existing file at the path.\n"

#define HELP_MESSAGE "\
tcomp - texture precompilation utility\n\
Processes various image formats into C header definitions.\n"

#define BYTE_DATA_START "\
#ifndef HL_COMPILE_RES\n\
extern\n\
#endif\n\
unsigned char %sImg_BYTES[%lu]\n\
#ifdef HL_COMPILE_RES\n\
={\n"

#define BYTE_DATA_END "\
\n}\n\
#endif\n\
;\n"

#define STRUCT "\
#ifndef HL_COMPILE_RES\n\
extern\n\
#endif\n\
HL_RES_IMAGE %sImg\n\
#ifdef HL_COMPILE_RES\n\
= {%sImg_BYTES,%i,%i,0,%i,GL_TEXTURE_2D}\n\
#endif\n\
;\n"

FILE* out;

char* pathGetName(char* path, char delimeter)
{
	uint i = 0;
	uint last = 0;
	
	while (1)
	{
		if (path[i] == 0) break;
		if (path[i] == delimeter) last = i;
		
		i++;
	}
	
	char* ret = (char*) malloc((i-last+1) * sizeof(char));
	last++;
	
	uint j = 0;	
	while (1)
	{
		if (last == i) break;
		
		// dont include the file extension in the name
		if (path[last] == '.') break;
		if (path[last] == '-' || path[last] == ' ')
		{
			ret[j] = '_';
			j++;
			last++;
			continue;
		}
		
		// if we want the extension as part of the variable name,
		// uncomment this part

//		if (path[last] != '.' && path[last] != '-')
//		{
			ret[j] = path[last];
			j++;
//		}

		last++;
	}
	ret[j] = 0;
	
	return ret;
}

void byteToStr(char* s, char b) {
	*((short*)s) = 0x7830 /*'0x'*/;	
	s+=2;
	for(int i = 0; i < 2; i++) {
		char a = (b >> ((i^1) << 2)) & 0xF;
		s[i] = a + (a < 10 ? '0' : ('A'-10));
	}
	s[2] = 0;
}

void expandFile(char* path)
{
	FILE* file = fopen(path, "r");
	if (!file)
	{
		fprintf(stderr, "Failed to open '%s'\n", path);
		return;
	}
	
	uint8* data;
	int width, height, channels;
	
	data = stbi_load_from_file(file, &width, &height, &channels, 0);
	if (!data)
	{
		fprintf(stderr, "Failed to load image '%s'\n", path);
		return;
	}
	
	char* name = pathGetName(path, PATH_DELIM);
	uint64 len = width * height * channels;
	
	//fprintf(out, "#ifndef HL_COMPILE_RES\nextern\n#endif\nstruct{unsigned char d[%lu];int w,h,b,c;unsigned int t;} %sImg\n#ifdef HL_COMPILE_RES\n= {\n{\n", len, name);
	
	fprintf(out, BYTE_DATA_START, name, len);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			char hex[5];
			sprintf(hex, "%u", data[y * width + x]);
			fprintf(out, "%s,", hex);
		}
	}
	
	fprintf(out, BYTE_DATA_END STRUCT, name, name, width, height, channels);
	
	//fprintf(out, "\n};\n#ifndef HL_COMPILE_RES\nextern\n#endif\nstruct{unsigned char d[%lu];int w,h,b,c;unsigned int t;} %sImg\n#ifdef HL_COMPILE_RES\n= {\n{\n"
	
	//fprintf(out, "},\n%i,%i,%i,0}\n#endif\n;\n", width, height, channels);
	
	return;
}

int main(int argc, char** argv)
{
	char* headerFile;
	
	if (argc < 3)
	{
		fprintf(stderr, HELP_MESSAGE USAGE);
		return 1;
	}
	
	headerFile = argv[1];
	
	FILE* header = fopen(headerFile, "w");
	if (!header)
	{
		fprintf(stderr, "Failed to create header\n");
		return 1;
	}
	out = header;
	
	fprintf(out, "\
	#include <glad/glad.h>\n\
	#include <GLFW/glfw3.h>\n\
	//#ifdef HL_COMPILE_RES\n\
	struct HL_RES_IMAGE {unsigned char* data; int width, height, depth; int channels; unsigned int type;};\n\
	//#endif\n");
	
	for (int i = 2; i < argc; i++)
	{
		char* inPath = argv[i];		
		expandFile(inPath);
	}
		
	return 0;
}
