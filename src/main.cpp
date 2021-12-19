/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 19:41:00 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/20 00:05:47 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gl-engine.h"
#include "CL/opencl.hpp"
#include <stdlib.h>
#include "CLContext.h"
#include "GLContext.h"
#include "ParticleSystem.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

class App : public glengine::Application
{
	private:
	glengine::Shader *s;
	glengine::Mesh *m;
	glengine::Entity *e;

	public:
	App(std::string title) : glengine::Application(title, 1280, 720) {}

	virtual void onAttach() override
	{
		s = new glengine::Shader("./res/shaders/phong.vert", "./res/shaders/phong.frag");
		m = glengine::Mesh::makeQuad();// glengine::loadObj("./res/obj/cube.obj");

		m->setVertexColors(glm::vec4(0.0, 1.0, 1.0, 1.0));
		
		e = new glengine::Entity(s, m);
		e->position = glm::vec3(0.0, 0.0, 0.0);
		e->scale = glm::vec3(10.0f, 10.0f, 10.0f);
		addEntity(e);

		camera->position = glm::vec3(0.0, 0.0, 20.0);
		
	}

	virtual void onUpdate(float deltaTime) override
	{
		e->rotation.y += 45.0 * deltaTime;
	}

	virtual void onDetach() override
	{
		delete m;
		delete s;
		delete e;
	}
};

static cl::Platform getPlatform()
{
	std::vector<cl::Platform> platforms;

	cl_int result = cl::Platform::get(&platforms);

	if (platforms.size() == 0)
	{
		std::cout << "CLPlatform::CLPlatform -- Error: No OpenCL platforms found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available platforms:" << std::endl;
	for (auto p : platforms)
	{
		std::cout << "\t" << p.getInfo<CL_PLATFORM_NAME>() << std::endl;
	}

	cl::Platform platform = platforms[0];
	std::cout << "Selected platform: " << platform.getInfo<CL_PLATFORM_VENDOR>() << ", " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
	return platform;
}

static cl::Device getDevice(cl::Platform &platform)
{
	std::vector<cl::Device> devices;

	cl_int result = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	if (devices.size() == 0)
	{
		std::cout << "CLDevice::CLDevice -- Error: No devices found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available devices:" << std::endl;
	for (auto d : devices)
	{
		std::cout << "\t" << d.getInfo<CL_DEVICE_NAME>() << std::endl;
	}

	cl::Device device = devices[0];
	std::cout << "Selected device: " << device.getInfo<CL_DEVICE_NAME>() << ", " << std::endl;
	return device;
}




int main(void)
{
	// App *app = new App("particle-system");
	// app->run();
	// delete app;

	GLContext gl = GLContext("particle-system", 1280, 720);
	
	cl::Platform plat = getPlatform();
	cl::Device dev = getDevice(plat);
	CLContext cl = CLContext(plat, dev);
	// cl::string src = loadKernelSource("./res/kernel/simple_add.cl").c_str();
	// cl.addSource(src);
	// cl.compileProgram();

	ParticleSystem ps = ParticleSystem(gl, cl);
	ps.init();

	gl.run(&ps);
	// cl_int result;

	// // Allocate space on device
	// cl::Buffer buffer_a(cl.ctx, CL_MEM_READ_WRITE, sizeof(int) * 10);
	// cl::Buffer buffer_b(cl.ctx, CL_MEM_READ_WRITE, sizeof(int) * 10);
	// cl::Buffer buffer_c(cl.ctx, CL_MEM_READ_WRITE, sizeof(int) * 10);
	
	// int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	// int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

	// // Create queue to push commands to device
	// cl.queue.enqueueWriteBuffer(buffer_a, CL_TRUE, 0, sizeof(int) * 10, A);
	// cl.queue.enqueueWriteBuffer(buffer_b, CL_TRUE, 0, sizeof(int) * 10, B);

	// // Run kernel (needs deprecated apis)
	// cl::Kernel kernel_add = cl::Kernel(cl.program, "simple_add", &result);
	// kernel_add.setArg(0, buffer_a);
	// kernel_add.setArg(1, buffer_b);
	// kernel_add.setArg(2, buffer_c);
	// printf("cl::Kernel error %d\n", result);
	// cl_int ret = cl.queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(10), cl::NullRange);

	// printf("%d\n", ret);
	// cl.queue.finish();

	// int C[10] = {-1};

	// cl.queue.enqueueReadBuffer(buffer_c, CL_TRUE, 0, sizeof(int) * 10, &C);
	// cl.queue.finish(); // ?

	// printf("Result:\n");
	// for (size_t i = 0; i < 10; i++)
	// {
	// 	printf("%d ", C[i]);
	// }
	// printf("\n");
	

	return (0);
}
