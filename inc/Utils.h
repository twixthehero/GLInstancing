#pragma once
#include <string>
#include <vector>
#include <glm\glm.hpp>

class Utils
{
public:
	static std::vector<std::string>& Split(const std::string& string, char delimiter, std::vector<std::string>& elements);
	static std::vector<std::string> Split(const std::string& string, char delimiter);

	static glm::vec3 HSVtoRGB(glm::vec3 hsv);
};