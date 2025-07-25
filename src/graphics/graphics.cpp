#include "graphics.h"
#include "../utils/precomp.h"

Graphics::Graphics(gsl::not_null<Window*> window) 
	: window_(window) {}

void Graphics::Init() {
	Initialize();
}