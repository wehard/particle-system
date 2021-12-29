#include "GLContext.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "ParticleSystem.h"

static void mouseCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos;
		double ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		printf("mouse screen: %f, %f\n", xpos, ypos);
	}
}

static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 matRotate = glm::eulerAngleXYZ( glm::radians(rotation.x),  glm::radians(rotation.y),  glm::radians(rotation.z));
	glm::mat4 m = matTranslate * matRotate * matScale;
	return (m);
}

GLContext::GLContext(std::string title, int width, int height) 
{
	if (!glfwInit())
	{
		std::cout << "GLFW failed to initialize!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_MULTISAMPLE);
	// glEnable(GL_FOG);
	// glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glClearColor(0, 0, 0, 1.f);

	camera = new glengine::Camera(45.0f, (float)1280 / (float)720);
	camera->position = glm::vec3(0.0, 0.0, 2.0);

	glfwSetMouseButtonCallback(window, mouseCallback);
	
	glfwSwapInterval(0);
}

GLContext::~GLContext() 
{
	glfwTerminate();
}

void GLContext::run(ParticleSystem *ps) 
{
	lastTime = glfwGetTime();
	double lastUpdateFpsTime = lastTime;
	int frameCount = 0;

	while (!glfwWindowShouldClose(window )&& glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		if (currentTime - lastUpdateFpsTime > 1.0)
		{
			int fps = frameCount / lastUpdateFpsTime;
			glfwSetWindowTitle(window, std::to_string(fps).c_str());
			lastUpdateFpsTime = currentTime;
		}
		double xpos;
		double ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		xpos -= (1280.0 / 2.0);
		ypos -= (720.0 / 2.0);

		ps->m_pos = glm::vec2(xpos / 1280.0, -ypos / 720.0);
		ps->m_pos *= 2.0;

		// Update particles
		ps->update(deltaTime);


		// Render here!
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ps->shader->use();
		ps->shader->setVec2("m_pos", ps->m_pos);
		ps->shader->setMat4("proj_matrix", camera->getProjectionMatrix());
		ps->shader->setMat4("view_matrix", camera->getViewMatrix());
		ps->shader->setMat4("model_matrix", getModelMatrix(glm::vec3(0), glm::vec3(0.0, 45.0, 0.0), glm::vec3(1.0)));

		glBindVertexArray(ps->vao);
		glBindBuffer(GL_ARRAY_BUFFER, ps->vbo);
		glDrawArrays(GL_POINTS, 0, ps->numParticles);


		glfwSwapBuffers(window);

		lastTime = currentTime;
		frameCount++;

		glfwPollEvents();
	}
}
