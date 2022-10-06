#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ext.h>

#define PATH_DELIM '/'
#ifdef _WIN32
	#define PATH_DELIM '\'
#endif

#define USAGE "\n\
Usage:\n\
scomp [HEADER_PATH] [IN_FILE1] [IN_FILE2] ...\n\
HEADER_PATH is a path to the header file to send output.\n\
This will overwrite any existing file at the path.\n"

#define HELP_MESSAGE "\
scomp - shader precompilation utility\n\
Processes GLSL shader code into C header definitions.\n\
\n\
Use '@import FILE_PATH'. (Must include space!)\n"

FILE* out;

char* pathGetDir(char* path, char delimeter)
{
	uint i = 0;
	uint last = 0;
	
	while (1)
	{
		if (path[i] == 0) break;
		if (path[i] == delimeter) last = i;
		
		i++;
	}
	
	char* ret = (char*) malloc((last+2) * sizeof(char));
	memcpy(ret, path, (last+1) * sizeof(char));
	ret[last+1] = 0;
	
	return ret;
}

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
		if (path[last] != '.' && path[last] != '-')
		{
			ret[j] = path[last];
			j++;
		}
		last++;
	}
	ret[j] = 0;
	
	return ret;
}

void expandFile(char* pwd, char* name)
{
	string path(pwd);
	path.append(name);
	
	FILE* file = fopen(path.str, "r");
	if (!file)
	{
		fprintf(stderr, "Failed to open '%s'\n", path.str);
		return;
	}
	
	fseek(file, 0, SEEK_SET);
	
	char c = fgetc(file);
	
	while (c != 0 && c != EOF)
	{
		if (c == '\n') fputs("\\\n\\n\t", out);
		
		else if (c == '@')
		{
			char* import = (char*) malloc(512 * sizeof(char));
			fscanf(file, "%s%s", import, import);
			expandFile(pathGetDir(path.str, PATH_DELIM), import);
			free(import);
		}
		
		else fputc(c, out);
		c = fgetc(file);
	}
	
	fclose(file);
	
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
	
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--help"))
		{
			printf(HELP_MESSAGE);
			return 0;
		}
	}
	
	FILE* header = fopen(headerFile, "w");
	if (!header)
	{
		fprintf(stderr, "Failed to create header\n");
		return 1;
	}
	out = header;
	
	for (int i = 2; i < argc; i++)
	{
		char* inPath = argv[i];
		char* name = pathGetName(inPath, PATH_DELIM);
		
		fprintf(out, "#ifndef HL_COMPILE_RES\nextern\n#endif\nchar* %sShaderCode\n#ifdef HL_COMPILE_RES\n= \n\"\\\n", name);
	
		expandFile("", inPath);
		
		fputs("\\\n\"\n#endif\n;\n", out);
	}
		
	return 0;
}
