/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:18:47 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/19 20:18:33 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CLContext.h"
#include <iostream>

CLContext::CLContext(cl::Platform &platform, cl::Device &device) : platform(platform), device(device)
{

	try
	{
		ctx = cl::Context({device});
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
	program.build({device});
	if (result != CL_SUCCESS)
	{
		std::cout << "CLContext::compileProgram -- Error" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Successully compiled kernel program." << std::endl;
}

CLContext::~CLContext() {}
