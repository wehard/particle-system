#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>


class ParticleSystem;

class GLContext
{
private:
	GLFWwindow *window;
	double	lastTime;
public:
	GLContext(std::string title, int width, int height);
	~GLContext();
	void run(ParticleSystem *ps);
};

