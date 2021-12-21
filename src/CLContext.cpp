/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:18:47 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/21 19:50:19 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CLContext.h"
#include <iostream>
#include "GL/glx.h"

CLContext::CLContext(cl::Platform &platform, cl::Device &device) : platform(platform), device(device)
{
	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
		0
	};

	try
	{
		ctx = cl::Context({device, &properties[0]});
		queue = cl::CommandQueue(ctx, device, cl::QueueProperties::OutOfOrder);
	}
	catch (const cl::Error &e)
	{
		std::cout << "cl::Error -- " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Successully created CL context." << std::endl;
	std::cout << "Successully created CL command queue." << std::endl;
}

void CLContext::addSource(cl::string source) 
{
	sources.push_back({source.c_str(), source.length()});
}

void CLContext::compileProgram() 
{
	cl_int result;
	program = cl::Program(ctx, sources, &result);
	if (result != CL_SUCCESS)
	{
		std::cout << "CLContext::compileProgram -- Program error!" << std::endl;
	}

	try
	{
		program.build({ device });
	}
	catch (const cl::Error & e)
	{
		(void)e;
		
		exit(EXIT_FAILURE);
		std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str() << std::endl;
	}

	std::cout << "Successully compiled kernel program." << std::endl;
}

CLContext::~CLContext() {}


void	CheckCLResult(int32_t result, std::string name)
{
	if (result != CL_SUCCESS)
	{
		std::cout << "OpenCL error: " << name << std::endl;
		exit(EXIT_FAILURE);
	}
}
