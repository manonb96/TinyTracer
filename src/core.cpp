#include "core.hpp"
#include "constants.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "raytracer.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

void Core::InitializeScene() {
	mainCamera = new Camera();
	mainSphere = new Sphere(float3(0.f, 0.f, 10.f), 2.f);
}

Core::~Core() {
	delete mainCamera;
	delete mainSphere;
}

float3 Core::CalculateColor(bool hitWithMain) {
	if (hitWithMain) 
        return mainSphere->color;
	else return float3(0.0f, 0.0f, 0.0f);
}

void Core::InitializeGLEW() {
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Core::InitializeFramebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer initialization failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Core::RenderScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH / 2; x++) {
            Ray* primaryRay = mainCamera->GeneratePrimaryRay(x, y);
            bool hit = mainSphere->IntersectRaySphere(*primaryRay);
            float3 color = CalculateColor(hit);

            glBegin(GL_POINTS);
            glColor3f(color.x, color.y, color.z);
            glVertex2f((float)x, (float)y);
            glEnd();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glfwSwapBuffers(glfwGetCurrentContext());
}