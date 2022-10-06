#pragma once

struct vec2 {float x, y;};
struct vec3 {float x, y, z;};
struct vec4 {float x, y, z, w;};

struct mat4 {float m0, m4, m8, m12,
			      m1, m5, m9, m13,
			      m2, m6, m10, m14,
			      m3, m7, m11, m15;};

void FileToString(char* file, char** string)
{
	FILE* f = fopen(file, "r");
	fseek(f, 0, SEEK_END);
	int len = ftell(f) + 1;
	fseek(f, 0, SEEK_SET);
	
	*string = (char*)malloc(len * sizeof(char));
	fread(*string, sizeof(char), len, f);
}

struct Shader
{
	unsigned int id;
	unsigned int vert;
	unsigned int frag;
	
	static uint textureSlot;
	
	char name[32];
	
	void init()
	{
		name[0] = '0';
	}
	
	void setName(const char* _name)
	{
		int i = 0;
		for (; *_name != '0'; _name++)
		{
			if (i > 30) break;
			name[i] = *_name;
			i++;
		}
		name[i] = '0';
	}
	
	void load(char* vertCode, char* fragCode)
	{
		int compile = 0;
		char err_str[1024];
		
		vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &vertCode, 0);
		glCompileShader(vert);
		glGetShaderiv(vert, GL_COMPILE_STATUS, &compile);
		if (!compile)
		{
			glGetShaderInfoLog(vert, 1024, 0, err_str);
			fprintf(stderr, "ERROR: [%s] vertex\n%s", name, err_str);
		}
		
		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &fragCode, NULL);
		glCompileShader(frag);
		glGetShaderiv(frag, GL_COMPILE_STATUS, &compile);
		if (!compile)
		{
			glGetShaderInfoLog(frag, 1024, 0, err_str);
			fprintf(stderr, "ERROR: [%s] fragment\n%s", name, err_str);
		}
		
		id = glCreateProgram();
		glAttachShader(id, vert);
		glAttachShader(id, frag);
		glLinkProgram(id);
		
		glDeleteShader(vert);
		glDeleteShader(frag);
	}
	
	void loadFile(char* vertPath, char* fragPath)
	{
		char* vertCode,
		    * fragCode;
		FileToString(vertPath, &vertCode);
		FileToString(fragPath, &fragCode);
		
		load(vertCode, fragCode);
	}
	
	void setInt(char* uniform, int value)
	{
		glUniform1i(glGetUniformLocation(id, uniform), value);
	}
	
	void setFloat(char* uniform, float value)
	{
		glUniform1f(glGetUniformLocation(id, uniform), value);
	}
	
	void setVec2(char* uniform, vec2 value)
	{
		glUniform2f(glGetUniformLocation(id, uniform), value.x, value.y);
	}
	
	void setVec3(char* uniform, vec3 value)
	{
		glUniform3f(glGetUniformLocation(id, uniform), value.x, value.y, value.z);
	}
	
	void setVec4(char* uniform, vec4 value)
	{
		glUniform4f(glGetUniformLocation(id, uniform), value.x, value.y, value.z, value.w);
	}
	
	void setMat4(char* uniform, mat4 value)
	{
		float matrixBuffer[16] =
		{
        value.m0, value.m1, value.m2, value.m3,
        value.m4, value.m5, value.m6, value.m7,
        value.m8, value.m9, value.m10, value.m11,
        value.m12, value.m13, value.m14, value.m15
    };
    glUniformMatrix4fv(glGetUniformLocation(id, uniform), 1, false, matrixBuffer);
	}
	
	void setTexture(char* uniform, Texture texture)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture((texture.img.depth) ? GL_TEXTURE_3D : GL_TEXTURE_2D, texture.id);
		glUniform1i(glGetUniformLocation(id, uniform), textureSlot);
		
		textureSlot++;
	}
	
	static void clear()
	{
		textureSlot = 0;
	}
	
	void use()
	{
		glUseProgram(id);
	}
};

uint Shader::textureSlot = 0;