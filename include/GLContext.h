#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

class ParticleSystem;

struct GLInfo
{
	GLint glMajorVersion;
	GLint glMinorVersion;
	GLint glslMajorVersion;
	GLint glslMinorVersion;
	const GLubyte *shadingLanguageVersion;
	const GLubyte *vendor;
	const GLubyte *renderer;
};

class GLContext
{
private:
	void readGLInfo();
	const char *glslVersion = "#version 150";
public:
	glm::vec4 clearColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
	GLInfo glInfo;
	GLFWwindow *window;
	double	lastTime;
	glm::mat4x4 projection;
	glm::mat4x4 view;
	glengine::Camera *camera;
	int width;
	int height;
	int fps;
	GLContext(std::string title, int width, int height);
	~GLContext();
	void run(ParticleSystem *ps);
	glm::vec3 GetMouseWorldCoord();
};

