#include "shader.hpp"

// ***********************************************************
// Source: https://learnopengl.com/Getting-started/Shaders
// ***********************************************************

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
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
        std::cerr << "[ERROR] FragmentShader file not successfully read" << std::endl;
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
		std::cerr << "[ERROR] Vertexshader compilation failed: \n" << infoLog << std::endl;
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
		std::cerr << "[ERROR] Fragmentshader compilation failed: \n" << infoLog << std::endl;
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
		std::cerr << "[ERROR] Shaderprogram failed: \n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	pixelColorLocation = glGetUniformLocation(ID, "pixelColor");
}

void Shader::use() {
	glUseProgram(ID);
}