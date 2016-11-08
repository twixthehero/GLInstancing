#pragma once
#include <string>
#include <map>
#include <glew\glew.h>
#include <glm\glm.hpp>
#include <vector>

class Shader
{
public:
	Shader(int id, std::string name);
	~Shader();

	void Bind();

	int GetUniformLocation(std::string uniform);
	int GetUniformLocationInstance(std::string uniform, int instance);

	void SetUniform1f(std::string uniform, float x);
	void SetUniform2f(std::string uniform, float x, float y);
	void SetUniform3f(std::string uniform, float x, float y, float z);
	void SetUniform4f(std::string uniform, float x, float y, float z, float w);

	void SetUniform2f(std::string uniform, glm::vec2 v);
	void SetUniform3f(std::string uniform, glm::vec3 v);
	void SetUniform4f(std::string uniform, glm::vec4 v);

	void SetUniformMatrix4f(std::string uniform, glm::mat4 m);
	void SetUniformMatrixInstance4f(std::string uniform, int instance, glm::mat4 m);
protected:
	std::string ReadText(std::string filename, std::vector<std::string>* uniforms);
	virtual void Load(std::string name);
	void AddUniform(std::string name);

	GLuint _program;
	std::map<std::string, GLuint> _uniforms;
private:
	int _id;
	std::string _name;

	std::vector<std::string> _uniformNames;
	std::vector<std::string> _uniformTypes;
};

