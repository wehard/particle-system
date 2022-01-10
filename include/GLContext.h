#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

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
public:
	const char *glslVersion = "#version 150";
	glm::vec4 clearColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
	GLInfo glInfo;
	GLFWwindow *window;
	glm::mat4x4 projection;
	glm::mat4x4 view;
	int width;
	int height;
	GLContext(std::string title, int width, int height);
	~GLContext();
	void Run(ParticleSystem *ps);
	glm::vec3 GetMouseWorldCoord(Camera *camera);
};

