#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(const string name, string vertexShaderPath, string fragmentShaderPath);
	~Shader();

	vector<uchar> GetVertexShaderBytes(bool addNullTerminator = false);
	vector<uchar> GetFragmentShaderBytes(bool addNullTerminato = false);
private:
	const string name_;
	string vertexShaderPath_;
	string fragmentShaderPath_;
};