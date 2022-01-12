#include "GLContext.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "ParticleSystem.h"
#include "GUIContext.h"
#include "GLObject.h"

static void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	auto ps = (ParticleSystem *)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		ps->AddGravityPoint();
	}
}

static void glfwMouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	auto ps = (ParticleSystem *)glfwGetWindowUserPointer(window);
	ps->camera.position.z += yoffset * 0.1f;
	printf("scroll x %f, y %f ", xoffset, yoffset);
	printf("camera z %f\n", ps->camera.position.z);
}

static void glfwMouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	auto ps = (ParticleSystem *)glfwGetWindowUserPointer(window);

	float offsetX = xpos - ps->mouseInfo.screen.x;
	float offsetY = ypos - ps->mouseInfo.screen.y;

	glm::vec2 offset = glm::vec2(offsetX, offsetY);

	if (ps->mouseMovement)
		ps->camera.Rotate(offsetX, offsetY, true);

	ps->mouseInfo.screen = glm::vec3(xpos, ypos, 0.0);
	ps->mouseInfo.ndc = glm::vec3((float)xpos / ((float)ps->gl.width * 0.5f) - 1.0f, (float)ypos / ((float)ps->gl.height * 0.5f) - 1.0f, 0.0);
	ps->mouseInfo.world = ps->gl.GetMouseWorldCoord(&ps->camera);
}

static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	auto ps = (ParticleSystem *)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		ps->mouseMovement = !ps->mouseMovement;
		if (ps->mouseMovement)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (key == GLFW_KEY_W)
	{
		ps->camera.Move(FORWARD, 0.25);
	}
	if (key == GLFW_KEY_S)
	{
		ps->camera.Move(BACKWARD, 0.25);
	}
	if (key == GLFW_KEY_A)
	{
		ps->camera.Move(LEFT, 0.25);
	}
	if (key == GLFW_KEY_D)
	{
		ps->camera.Move(RIGHT, 0.25);
	}
}

static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

GLContext::GLContext(std::string title, int width, int height) : width(width), height(height)
{
	if (!glfwInit())
	{
		std::cout << "GLFW failed to initialize!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	this->glslVersion = "#version 150";

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		std::cout << "GLFW failed to create window!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
	glfwSetScrollCallback(window, glfwMouseScrollCallback);
	glfwSetKeyCallback(window, glfwKeyCallback);
	glfwSetCursorPosCallback(window, glfwMouseCallback);

	glfwSwapInterval(0);
	readGLInfo();
}

void GLContext::readGLInfo()
{
	glGetIntegerv(GL_MAJOR_VERSION, &glInfo.glMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &glInfo.glMinorVersion);
	glInfo.vendor = glGetString(GL_VENDOR);
	glInfo.renderer = glGetString(GL_RENDERER);
	glInfo.shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
}

GLContext::~GLContext()
{
	glfwTerminate();
}

static glm::vec3 intersect(glm::vec3 planeP, glm::vec3 planeN, glm::vec3 rayP, glm::vec3 rayD)
{
	float d = glm::dot(planeP, -planeN);
	float t = -(d + rayP.z * planeN.z + rayP.y * planeN.y + rayP.x * planeN.x) / (rayD.z * planeN.z + rayD.y * planeN.y + rayD.x * planeN.x);
	return rayP + t * rayD;
}

static glm::vec3 projectMouse(int mouseX, int mouseY, float width, float height, glm::mat4 proj, glm::mat4 view)
{
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	float z = 1.0f;

	glm::vec3 ndc = glm::vec3(x, y, z);

	// homogenous clip space
	glm::vec4 clip = glm::vec4(ndc.x, ndc.y, -1.0, 1.0);

	// eye space
	glm::vec4 eye = glm::inverse(proj) * clip;

	eye = glm::vec4(eye.x, eye.y, -1.0, 0.0);

	glm::vec4 temp = glm::inverse(view) * eye;
	glm::vec3 world = glm::vec3(temp.x, temp.y, temp.z);

	world = glm::normalize(world);

	return world;
}

glm::vec3 GLContext::GetMouseWorldCoord(Camera *camera)
{
	glm::vec3 world;
	double xpos;
	double ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	auto pPos = glm::vec3(0.0);
	auto pNormal = glm::normalize(camera->position - pPos);

	world = intersect(pPos, pNormal, camera->position, projectMouse(xpos, ypos, width, height, camera->getProjectionMatrix(), camera->getViewMatrix()));
	return world;
}
