#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

class ParticleSystem;

class GLContext
{
private:
	GLFWwindow *window;
	double	lastTime;
	glm::mat4x4 projection;
	glm::mat4x4 view;
	glengine::Camera *camera;
	int width;
	int height;
public:
	GLContext(std::string title, int width, int height);
	~GLContext();
	void run(ParticleSystem *ps);
};

