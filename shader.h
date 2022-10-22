#pragma once

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
	uint id;
	uint vert;
	uint frag;
	
	static Shader* activeShader;
	static uint textureSlot;
	
	char name[32];
	
	void init()
	{
		name[0] = 0;
	}
	
	void setName(const char* _name)
	{
		int i = 0;
		for (; *_name != 0; _name++)
		{
			if (i > 30) break;
			name[i] = *_name;
			i++;
		}
		name[i] = 0;
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
		float* matrixBuffer = &value[0][0];
		glUniformMatrix4fv(glGetUniformLocation(id, uniform), 1, false, matrixBuffer);
	}
	
	void setTexture(char* uniform, Texture texture)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture((texture.img.depth) ? GL_TEXTURE_3D : GL_TEXTURE_2D, texture.id);
		glUniform1i(glGetUniformLocation(id, uniform), textureSlot);
		
		textureSlot++;
	}
};

Shader* Shader::activeShader = (Shader*)0;
uint Shader::textureSlot = 0;

inline
Shader* activeShader() { return Shader::activeShader; }

void clearShaders()
{
	Shader::textureSlot = 0;
}

void useShader(Shader* shader)
{
	Shader::activeShader = shader;
	glUseProgram(shader->id);
	clearShaders();
}