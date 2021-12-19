#include "GLContext.h"
#include <iostream>

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
	
	glfwSwapInterval(0);
}

GLContext::~GLContext() 
{
	glfwTerminate();
}

void GLContext::run() 
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

		// Render here!

		glfwSwapBuffers(window);

		lastTime = currentTime;
		frameCount++;

		glfwPollEvents();
	}
}
