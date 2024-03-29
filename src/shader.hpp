#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "base_types.hpp"

class Shader {
public:
	unsigned int ID;
	int pixelColorLocation;
	Shader(const char* vertexPath, const char* fragmentPath);
	void use();
};