#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../utils/precomp.h"

class Shader {
public:
	Shader(const std::string name, std::string vertexShaderPath, std::string fragmentShaderPath);
	~Shader();

	std::vector<std::uint8_t> GetVertexShaderBytes(bool addNullTerminator = false);
	std::vector<std::uint8_t> GetFragmentShaderBytes(bool addNullTerminato = false);
private:
	const std::string name_;
	std::string vertexShaderPath_;
	std::string fragmentShaderPath_;
};