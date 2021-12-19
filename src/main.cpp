/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 19:41:00 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/19 03:29:26 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gl-engine.h"
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include "CL/opencl.hpp"
#include <stdlib.h>

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


int main(void)
{
	// App *app = new App("particle-system");
	// app->run();
	// delete app;
	int platformIndex = 0;
	cl_int result;

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() == 0)
	{
		printf("OpenCL: No platforms found!");
		exit(EXIT_FAILURE);
	}
	cl::Platform default_platform = platforms[0];
	printf("Running on platform: %s\n", default_platform.getInfo<CL_PLATFORM_NAME>().c_str());


	std::vector<cl::Device> all_devices;
	result = default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0)
	{
		printf("OpenCL: No devices found.");
		exit(EXIT_FAILURE);
	}
	printf("getDevice: %d\n", result);
	cl::Device default_device = all_devices[0];
	printf("Using device: %s\n", default_device.getInfo<CL_DEVICE_NAME>().c_str());

	cl::Context context({default_device});
	cl::Program::Sources sources;
	// cl::string kernel_code =
	// 		"void kernel simple_add(global int* A, global int* B, global int* C)"
	// 		"{"
	// 		"  C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];"
	// 		"  A[get_global_id(0)] = 99;"
	// 		"}";

	cl::string kernel_code =
			"__kernel void simple_add(__global int* A, __global int* B, __global int* C)"
			"{"
			"  C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];"
			"printf(\"hello\\n\");"
			"}";

	sources.push_back({kernel_code.c_str(), kernel_code.length()});


	cl::Program program(context, sources, &result);
	printf("program: %d\n", result);
	result = program.build({default_device});
	if (result != CL_SUCCESS)
	{
		printf("OpenCL: Error building: %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device).c_str());
		exit(EXIT_FAILURE);
	}
	printf("program.build: %d\n", result);

	// Allocate space on device
	cl::Buffer buffer_a(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	cl::Buffer buffer_b(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	cl::Buffer buffer_c(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
	
	int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

	// Create queue to push commands to device
	cl::CommandQueue queue(context, default_device, cl::QueueProperties::OutOfOrder, &result);
	printf("queue construct: %d\n", result);
	queue.enqueueWriteBuffer(buffer_a, CL_TRUE, 0, sizeof(int) * 10, A);
	queue.enqueueWriteBuffer(buffer_b, CL_TRUE, 0, sizeof(int) * 10, B);

	// Run kernel (needs deprecated apis)
	cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
	kernel_add.setArg(0, buffer_a);
	kernel_add.setArg(1, buffer_b);
	kernel_add.setArg(2, buffer_c);
	cl_int ret = queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(10));

	printf("%d\n", ret);
	queue.finish();

	int C[10] = {-1};

	queue.enqueueReadBuffer(buffer_c, CL_TRUE, 0, sizeof(int) * 10, &C);
	queue.finish(); // ?

	printf("Result:\n");
	for (size_t i = 0; i < 10; i++)
	{
		printf("%d ", C[i]);
	}
	printf("\n");
	

	return (0);
}
