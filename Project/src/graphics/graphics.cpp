#include "graphics.hpp"

Graphics::Graphics(gsl::not_null<Window*> window, Shader* shader) 
	: window_(window), shader_(shader) {}

void Graphics::Init() {
	Initialize();
}