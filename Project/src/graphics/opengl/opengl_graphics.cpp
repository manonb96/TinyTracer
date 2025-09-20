#include "opengl_graphics.hpp"

#pragma region Helpers

void FramebufferSizeCallback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
}

#pragma endregion

#pragma region Graphics Pipeline
void OpenGLGraphics::AttachShader()
{
	vector<uchar> vertexShaderBytes = m_pShader->GetVertexShaderBytes(true);
	vector<uchar> fragmentShaderBytes = m_pShader->GetFragmentShaderBytes(true);

	cstring vertexShaderData = reinterpret_cast<cstring>(vertexShaderBytes.data());
	cstring fragmentShaderData = reinterpret_cast<cstring>(fragmentShaderBytes.data());

	// Set up shader
	uint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderData, NULL);
	glCompileShader(vertexShader);

	int vsSuccess;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vsSuccess);
	if (!vsSuccess) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		spdlog::error("[OpenGL Error] Vertex shader compilation failed: {}", infoLog);
	}

	uint fragmentShader;
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

	uint shaderProgram = glCreateProgram();
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
	glfwSwapBuffers(m_pWindow->GetHandle());
}

#pragma endregion

#pragma region Buffers

void OpenGLGraphics::BindVertexArrayObject() {
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
}

void OpenGLGraphics::CreateVertexBuffer(span<Vertex> vertices) {
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float3)));
	glEnableVertexAttribArray(1);
}

void OpenGLGraphics::CreateIndexBuffer(span<int> indices) {
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}

void OpenGLGraphics::RenderIndexedBuffer() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLGraphics::SetViewProjection(mat4 view, mat4 projection) { }

#pragma endregion

#pragma region Texture

void OpenGLGraphics::CreateTexture(uchar* pixels)
{
	m_pPixels = pixels;
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pPixels);
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
	glfwGetFramebufferSize(m_pWindow->GetHandle(), &width, &height);
	glViewport(0, 0, width, height); // TODO: Handle viewport size in more robust manner
	glfwSetFramebufferSizeCallback(m_pWindow->GetHandle(), FramebufferSizeCallback);

	// Bind vertex array object
	BindVertexArrayObject();

	// Set shader
	AttachShader();
}

#pragma endregion