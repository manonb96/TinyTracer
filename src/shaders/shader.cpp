#include "shader.hpp"
#include <spdlog/spdlog.h>

// ***********************************************************
// Source: https://learnopengl.com/Getting-started/Shaders
// ***********************************************************

std::string GetCompileShaderCommand(const char* path) {
	return std::format("glslc {} -o {}.spv", path, path);
}

// TODO: Refactor
Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {

#if OPENGL
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
		spdlog::error("Could not read shader files");
    }

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

	// Set up shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int vsSuccess;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vsSuccess);
	if (!vsSuccess) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		spdlog::error("[OpenGL Error] Vertex shader compilation failed: {}", infoLog);
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int fsSuccess;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fsSuccess);
	if (!fsSuccess) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		spdlog::error("[OpenGL Error] Fragment shader compilation failed: {}", infoLog);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	int spSuccess;
	glGetProgramiv(ID, GL_LINK_STATUS, &spSuccess);
	if (!spSuccess) {
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		spdlog::error("[OpenGL Error] Shaderprogram failed: {}", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	pixelColorLocation = glGetUniformLocation(ID, "pixelColor");
#elif VULKAN
	ID = 0;
	
	// Create SPIR-V files
	std::string compileVertexShaderCommand = GetCompileShaderCommand(vertexShaderPath);

	if (system(compileVertexShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Vertex shader compilation failed");
	}

	std::string compileFragmentShaderCommand = GetCompileShaderCommand(fragmentShaderPath);
	if (system(compileFragmentShaderCommand.c_str()) != 0) {
		spdlog::error("[Vulkan Error] Fragment shader compilation failed");
	}
#endif
}

void Shader::use() {
	glUseProgram(ID);
}