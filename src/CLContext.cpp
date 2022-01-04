/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:18:47 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/21 23:07:06 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CLContext.h"
#include <iostream>
#ifdef LINUX
#include "GL/glxint.h" Linux only
#endif

#if defined(__APPLE__)
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/OpenGL.h>
#endif

static void CheckCLResult(int32_t result, std::string name)
{
	if (result != CL_SUCCESS)
	{
		std::cout << "OpenCL error: " << name << std::endl;
		exit(EXIT_FAILURE);
	}
}

CLContext::CLContext(cl_platform_id platform, cl_device_id device) : platform(platform), device(device)
{
#ifdef LINUX
	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
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
	// ctx = cl::Context({device, &properties[0]});

	ctx = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &result);
	// CheckCLResult(result, "clCreateContext");
	// CheckCLResult(clGetContextInfo(ctx, CL_CONTEXT_NUM_DEVICES, sizeof(cl_int), &(this->numDevices), NULL),"clGetContextInfo");
	queue = clCreateCommandQueue(ctx, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &result);
	CheckCLResult(result, "create command queue");
	// try
	// {
	// }

	// catch (const cl::Error &e)
	// {
	// 	std::cout << "cl::Error -- " << e.what() << std::endl;
	// 	exit(EXIT_FAILURE);
	// }
	std::cout << "Successully created CL context." << std::endl;
	std::cout << "Successully created CL command queue." << std::endl;
}

void CLContext::addSource(std::string source)
{
	sources.push_back({source.c_str(), source.length()});
}

void CLContext::compileProgram()
{
	size_t program_size = 0;
	cl_int error = CL_SUCCESS;

	char** result = new char*[sources.size()];
	for (int index = 0; index < sources.size(); index++) {
		program_size += sources[index].size();
	    result[index] = const_cast<char*>(sources[index].c_str());
	}

	// std::string source = sources.pop_back();
	program = clCreateProgramWithSource(ctx, 1, (const char **)result, &program_size, &error);

	// cl_int result;
	// program = cl::Program(ctx, sources, &result);
	// if (result != CL_SUCCESS)
	// {
	// 	std::cout << "CLContext::compileProgram -- Program error!" << std::endl;
	// }

	// try
	// {
	// 	program.build({device});
	// }
	// catch (const cl::Error &e)
	// {
	// 	(void)e;

	// 	exit(EXIT_FAILURE);
	// 	std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str() << std::endl;
	// }

	std::cout << "Successully compiled kernel program." << std::endl;
}

CLContext::~CLContext() {}
