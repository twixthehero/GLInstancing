#include "Shader.h"
#include <fstream>
#include <iostream>
#include "Utils.h"
#include <glew\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Utils.h"

Shader::Shader(int id, std::string name)
{
	_id = id;
	_name = name;

	_uniforms = std::map<std::string, GLuint>();
	_uniformNames = std::vector<std::string>();
	_uniformTypes = std::vector<std::string>();

	Load(_name);
}


Shader::~Shader()
{
}

void Shader::Bind()
{
	glUseProgram(_program);
}

int Shader::GetUniformLocation(std::string uniform)
{
	if (_uniforms.find(uniform) != _uniforms.end())
		return _uniforms[uniform];

	return -1;
}

int Shader::GetUniformLocationInstance(std::string uniform, int instance)
{
	std::string name = uniform + "[" + std::to_string(instance) + "]";

	GLuint location = GetUniformLocation(name);

	if (location == -1)
	{
		AddUniform(name);
		return _uniforms[name];
	}
	
	return location;
}

void Shader::SetUniform1f(std::string uniform, float x)
{
	glUniform1f(GetUniformLocation(uniform), x);
}

void Shader::SetUniform2f(std::string uniform, float x, float y)
{
	glUniform2f(GetUniformLocation(uniform), x, y);
}

void Shader::SetUniform3f(std::string uniform, float x, float y, float z)
{
	glUniform3f(GetUniformLocation(uniform), x, y, z);
}

void Shader::SetUniform4f(std::string uniform, float x, float y, float z, float w)
{
	glUniform4f(GetUniformLocation(uniform), x, y, z, w);
}

void Shader::SetUniform2f(std::string uniform, glm::vec2 v)
{
	SetUniform2f(uniform, v.x, v.y);
}

void Shader::SetUniform3f(std::string uniform, glm::vec3 v)
{
	SetUniform3f(uniform, v.x, v.y, v.z);
}

void Shader::SetUniform4f(std::string uniform, glm::vec4 v)
{
	SetUniform4f(uniform, v.x, v.y, v.z, v.w);
}

void Shader::SetUniformMatrix4f(std::string uniform, glm::mat4 m)
{
	glUniformMatrix4fv(GetUniformLocation(uniform), 1, false, &m[0][0]);
}

void Shader::SetUniformMatrixInstance4f(std::string uniform, int instance, glm::mat4 m)
{
	glUniformMatrix4fv(GetUniformLocationInstance(uniform, instance), 1, false, &m[0][0]);
}

std::string Shader::ReadText(std::string filename, std::vector<std::string>* uniforms)
{
	std::ifstream file("shaders\\" + filename);
	std::string line;
	std::string text;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.find("uniform") == 0)
			{
				std::vector<std::string> parts = Utils::Split(line, ' ');

				std::string uniformType = parts[1];
				std::string uniformName = parts[2].substr(0, parts[2].length() - 1);

				std::cout << "new uniform: " << uniformType << " " << uniformName << std::endl;

				uniforms->push_back(uniformName);
				_uniformNames.push_back(uniformName);
				_uniformTypes.push_back(uniformType);
			}

			text += line + "\n";
		}

		file.close();
	}

	return text;
}

void Shader::Load(std::string name)
{
	std::cout << "Loading shader: " << name << std::endl;
	std::vector<std::string>* uniforms = new std::vector<std::string>();
	std::string v = ReadText(name + ".vs", uniforms);
	std::string f = ReadText(name + ".fs", uniforms);
	const char* vert = v.c_str();
	const char* frag = f.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vert, NULL);
	glCompileShader(vs);

	int success = -1;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE)
	{
		std::cout << "ERROR: GL vertex shader index " << vs << " did not compile" << std::endl;

		int logLength;

		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLength);
		
		if (logLength > 0)
		{
			char* errorMessage = new char[logLength + 1];
			glGetShaderInfoLog(vs, logLength, NULL, errorMessage);
			std::cout << errorMessage << std::endl;
			delete errorMessage;
		}

		exit(-1);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &frag, NULL);
	glCompileShader(fs);

	success = -1;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE)
	{
		std::cout << "ERROR: GL fragment shader index " << fs << " did not compile" << std::endl;

		int logLength;

		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0)
		{
			char* errorMessage = new char[logLength + 1];
			glGetShaderInfoLog(vs, logLength, NULL, errorMessage);
			std::cout << errorMessage << std::endl;
			delete errorMessage;
		}

		exit(-1);
	}

	//std::cout << "Reserving pointer for shader" << std::endl;
	_program = glCreateProgram();

	if (_program == 0)
	{
		std::cout << "ERROR: Unable to create program space for shader" << std::endl;
		exit(-1);
	}

	//std::cout << "Linking vertex and fragment" << std::endl;
	glAttachShader(_program, vs);
	glAttachShader(_program, fs);
	glLinkProgram(_program);

	glGetProgramiv(_program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		std::cout << "ERROR: shader '" << name << "' did not link properly" << std::endl;
		exit(-1);
	}

	glDetachShader(_program, vs);
	glDetachShader(_program, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	//add all uniforms to map
	while (uniforms->size() > 0)
	{
		AddUniform(uniforms->at(uniforms->size() - 1));
		uniforms->pop_back();
	}

	for (int i = 0; i < _uniformNames.size(); i++)
		std::cout << _uniformNames[i] << " location: " << GetUniformLocation(_uniformNames[i]) << std::endl;

	delete uniforms;
}

void Shader::AddUniform(std::string name)
{
	if (_uniforms.find(name) == _uniforms.end())
	{
		std::cout << "adding uniform: " << name << std::endl;
		_uniforms.insert(std::pair<std::string, int>(name, glGetUniformLocation(_program, name.c_str())));
	}
}