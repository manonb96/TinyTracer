#include "graphics.hpp"

Graphics::Graphics(gsl::not_null<Window*> window, Shader* shader) 
	: m_pWindow(window), m_pShader(shader) {}

void Graphics::Init() {
	Initialize();
}