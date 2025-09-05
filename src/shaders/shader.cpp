#include "shader.hpp"
#include <spdlog/spdlog.h>

// ***********************************************************
// Source: https://learnopengl.com/Getting-started/Shaders
// ***********************************************************

std::string GetCompileShaderCommand(const char* path) {
	return std::format("glslc {} -o {}.spv", path, path);
}

Shader::Shader(const std::string name, std::string vertexShaderPath, std::string fragmentShaderPath)
	: name_(name), vertexShaderPath_(vertexShaderPath), fragmentShaderPath_(fragmentShaderPath) {

#if OPENGL
	// TODO: Preprocess files to add #include
#elif VULKAN
	// Create SPIR-V files
	std::string compileVertexShaderCommand = GetCompileShaderCommand(vertexShaderPath.c_str());

	if (system(compileVertexShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Vertex shader compilation failed");
	}

	std::string compileFragmentShaderCommand = GetCompileShaderCommand(fragmentShaderPath.c_str());
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

std::vector<std::uint8_t> Shader::GetVertexShaderBytes(bool addNullTerminator) {
	return ReadFile(vertexShaderPath_, addNullTerminator);
}

std::vector<std::uint8_t> Shader::GetFragmentShaderBytes(bool addNullTerminator) {
	return ReadFile(fragmentShaderPath_, addNullTerminator);
}