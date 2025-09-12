#include "opengl_graphics.hpp"

#pragma region Helpers

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
}

#pragma endregion

#pragma region Graphics Pipeline
void OpenGLGraphics::AttachShader()
{
	vector<uchar> vertexShaderBytes = shader_->GetVertexShaderBytes(true);
	vector<uchar> fragmentShaderBytes = shader_->GetFragmentShaderBytes(true);

	cstring vertexShaderData = reinterpret_cast<cstring>(vertexShaderBytes.data());
	cstring fragmentShaderData = reinterpret_cast<cstring>(fragmentShaderBytes.data());

	// Set up shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderData, NULL);
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
	glShaderSource(fragmentShader, 1, &fragmentShaderData, NULL);
	glCompileShader(fragmentShader);

	int fsSuccess;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fsSuccess);
	if (!fsSuccess) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		spdlog::error("[OpenGL Error] Fragment shader compilation failed: {}", infoLog);
	}

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int spSuccess;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &spSuccess);
	if (!spSuccess) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		spdlog::error("[OpenGL Error] Shaderprogram failed: {}", infoLog);
	}

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

#pragma endregion

#pragma region Drawing
bool OpenGLGraphics::BeginFrame() {
	return true;
}

void OpenGLGraphics::EndFrame() {
	glfwSwapBuffers(window_->GetHandle());
}

#pragma endregion

#pragma region Buffers

void OpenGLGraphics::BindVertexArrayObject() {
	glGenVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);
}

void OpenGLGraphics::CreateVertexBuffer(span<Vertex> vertices) {
	glGenBuffers(1, &VBO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float3)));
	glEnableVertexAttribArray(1);
}

void OpenGLGraphics::CreateIndexBuffer(span<int> indices) {
	glGenBuffers(1, &EBO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}

void OpenGLGraphics::RenderIndexedBuffer() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLGraphics::SetViewProjection(mat4 view, mat4 projection) { }

#pragma endregion

#pragma region Texture

void OpenGLGraphics::CreateTexture(uchar* pixels)
{
	pixels_ = pixels;
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_);
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

#pragma endregion

#pragma region Class

OpenGLGraphics::OpenGLGraphics(gsl::not_null<Window*> window, Shader* shader) : Graphics(window, shader) {
}

OpenGLGraphics::~OpenGLGraphics() {

}

void OpenGLGraphics::Initialize() {
	// Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::exit(EXIT_FAILURE); // TODO: Verify if this calls SPDLog
	}

	glClearColor(0.f, 0.f, 0.f, 1.0f);

	int width, height;
	glfwGetFramebufferSize(window_->GetHandle(), &width, &height);
	glViewport(0, 0, width, height); // TODO: Handle viewport size in more robust manner
	glfwSetFramebufferSizeCallback(window_->GetHandle(), framebuffer_size_callback);

	// Bind vertex array object
	BindVertexArrayObject();

	// Set shader
	AttachShader();
}

#pragma endregion