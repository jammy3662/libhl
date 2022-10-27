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
	
	char name[32];
	
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
	
	void setTexture(char* uniform, Texture& texture)
	{
		activateTexture(texture);
		glUniform1i(glGetUniformLocation(id, uniform), textureSlot);
		
		textureSlot++;
		//glActiveTexture(0); // so we dont accidentally modify this texture with later operations
	}
};

Shader* activeShader = 0;

Shader createShader(char* vertCode, char* fragCode)
{
	Shader ret;
	
	ret.name[0] = 0; // null terminate for safety
	
	int compile = 0;
	char err_str[1024];
	
	ret.vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ret.vert, 1, &vertCode, 0);
	glCompileShader(ret.vert);
	glGetShaderiv(ret.vert, GL_COMPILE_STATUS, &compile);
	if (!compile)
	{
		glGetShaderInfoLog(ret.vert, 1024, 0, err_str);
		fprintf(stderr, "ERROR: [%s] vertex\n%s", ret.name, err_str);
	}
	
	ret.frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ret.frag, 1, &fragCode, NULL);
	glCompileShader(ret.frag);
	glGetShaderiv(ret.frag, GL_COMPILE_STATUS, &compile);
	if (!compile)
	{
		glGetShaderInfoLog(ret.frag, 1024, 0, err_str);
		fprintf(stderr, "ERROR: [%s] fragment\n%s", ret.name, err_str);
	}
	
	ret.id = glCreateProgram();
	glAttachShader(ret.id, ret.vert);
	glAttachShader(ret.id, ret.frag);
	glLinkProgram(ret.id);
	
	glDeleteShader(ret.vert);
	glDeleteShader(ret.frag);
	
	return ret;
}

Shader createShaderFromFile(char* vertPath, char* fragPath)
{
	char* vertCode;
	char* fragCode;
	FileToString(vertPath, &vertCode);
	FileToString(fragPath, &fragCode);
	
	return createShader(vertCode, fragCode);
}

void useShader(Shader* shader)
{
	activeShader = shader;
	glUseProgram(shader->id);
}

Shader hl_textureShader;