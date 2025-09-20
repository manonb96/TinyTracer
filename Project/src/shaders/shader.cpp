#include "shader.hpp"

string GetCompileShaderCommand(cstring path) {
	return std::format("glslc {} -o {}.spv", path, path);
}

Shader::Shader(const string name, string vertexShaderPath, string fragmentShaderPath)
	: m_name(name) {
	m_vertexShaderPath = SHADER_DIR + vertexShaderPath;
	m_fragmentShaderPath = SHADER_DIR + fragmentShaderPath;

#if OPENGL
	// TODO: Preprocess files to add #include
#elif VULKAN
	// Create SPIR-V files
	string compileVertexShaderCommand = GetCompileShaderCommand(m_vertexShaderPath.c_str());

	if (system(compileVertexShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Vertex shader compilation failed");
	}

	string compileFragmentShaderCommand = GetCompileShaderCommand(m_fragmentShaderPath.c_str());
	if (system(compileFragmentShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Fragment shader compilation failed");
	}

	// Set paths to created SPIR-V files
	m_vertexShaderPath = m_vertexShaderPath + ".spv";
	m_fragmentShaderPath = m_fragmentShaderPath + ".spv";
#endif
}

Shader::~Shader() {
}

vector<uchar> Shader::GetVertexShaderBytes(bool addNullTerminator) {
	return ReadFile(m_vertexShaderPath, addNullTerminator);
}

vector<uchar> Shader::GetFragmentShaderBytes(bool addNullTerminator) {
	return ReadFile(m_fragmentShaderPath, addNullTerminator);
}