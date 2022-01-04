/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:21:55 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/21 23:06:06 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300

// #include "CL/opencl.hpp"
#include "OpenCL/opencl.h"
#include <string>
#include <vector>

class CLContext
{
private:
	// cl::Platform &platform;
	cl_platform_id platform;
	// cl::Device &device;
	cl_device_id device;
	cl_int numDevices;
public:
	// cl::Context ctx;
	cl_context ctx;
	// cl_context ctx;
	// cl::CommandQueue queue;
	cl_command_queue queue;
	std::vector<std::string> sources;
	// cl::Program::Sources sources;
	cl_program program;
	// cl::Program program;
	// cl_program program;
	CLContext(cl_platform_id platform, cl_device_id device);
	void addSource(std::string source);
	void compileProgram();
	~CLContext();
};
