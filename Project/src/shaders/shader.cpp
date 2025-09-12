#include "shader.hpp"

string GetCompileShaderCommand(cstring path) {
	return std::format("glslc {} -o {}.spv", path, path);
}

Shader::Shader(const string name, string vertexShaderPath, string fragmentShaderPath)
	: name_(name) {
	vertexShaderPath_ = SHADER_DIR + vertexShaderPath;
	fragmentShaderPath_ = SHADER_DIR + fragmentShaderPath;

#if OPENGL
	// TODO: Preprocess files to add #include
#elif VULKAN
	// Create SPIR-V files
	string compileVertexShaderCommand = GetCompileShaderCommand(vertexShaderPath_.c_str());

	if (system(compileVertexShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Vertex shader compilation failed");
	}

	string compileFragmentShaderCommand = GetCompileShaderCommand(fragmentShaderPath_.c_str());
	if (system(compileFragmentShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Fragment shader compilation failed");
	}

	// Set paths to created SPIR-V files
	vertexShaderPath_ = vertexShaderPath_ + ".spv";
	fragmentShaderPath_ = fragmentShaderPath_ + ".spv";
#endif
}

Shader::~Shader() {
}

vector<uchar> Shader::GetVertexShaderBytes(bool addNullTerminator) {
	return ReadFile(vertexShaderPath_, addNullTerminator);
}

vector<uchar> Shader::GetFragmentShaderBytes(bool addNullTerminator) {
	return ReadFile(fragmentShaderPath_, addNullTerminator);
}