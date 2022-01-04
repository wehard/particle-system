/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:18:47 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/05 00:20:10 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CLContext.h"
#include <iostream>
#ifdef __linux__
#include "CL/opencl.hpp"
#include "GL/glx.h"
#endif

#if defined(__APPLE__)
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/OpenGL.h>
#endif

static void CheckCLResult(int32_t result, std::string name)
{
	if (result != CL_SUCCESS)
	{
		std::cout << "OpenCL error " << result <<  " : "  << name << std::endl;
		exit(EXIT_FAILURE);
	}
}

CLContext::CLContext(cl_platform_id platform, cl_device_id device) : platform(platform), device(device)
{
#ifdef __linux__
	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		0};
#endif

#if defined(__APPLE__)

	CGLContextObj glContext = CGLGetCurrentContext();
	CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
	cl_context_properties properties[] = {
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
		(cl_context_properties)shareGroup,
		0};
#endif
	cl_int result;
	cl_uint num_devices;

	ctx = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &result);
	CheckCLResult(result, "clCreateContext");

	std::cout << "Successully created CL context." << std::endl;
	result = clGetContextInfo(ctx, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, NULL);
	CheckCLResult(result, "clGetContextInfo");
	printf("\tNum devices: %d\n", num_devices);

	queue = clCreateCommandQueue(this->ctx, device, 0, &result);
	CheckCLResult(result, "clCreateCommandQueue");

	std::cout << "Successully created CL command queue." << std::endl;
}

void CLContext::addSource(std::string source)
{
	this->source = source;
}

void CLContext::compileProgram()
{
	cl_int error = CL_SUCCESS;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;

	program_buffer = (char*)source.c_str();
	program_size = source.size();

	// program_handle = fopen("res/kernel/particles.cl", "r");
	// fseek(program_handle, 0, SEEK_END);
	// program_size = ftell(program_handle);
	// rewind(program_handle);
	// program_buffer = (char*)malloc(program_size + 1); program_buffer[program_size] = '\0'; fread(program_buffer, sizeof(char), program_size, program_handle);
	// fclose(program_handle);

	program = clCreateProgramWithSource(ctx, 1, (const char**)&program_buffer, NULL, &error);
	CheckCLResult(error, "clCreateProgramWithSource");

	error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	CheckCLResult(error, "clBuildProgram");

	std::cout << "Successully compiled kernel program." << std::endl;
}

CLContext::~CLContext() {}
