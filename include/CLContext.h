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

#include "CL/opencl.hpp"

class CLContext
{
private:
	cl::Platform &platform;
	cl::Device &device;
	cl_int numDevices;
public:
	cl::Context ctx;
	// cl_context ctx;
	// cl::CommandQueue queue;
	cl_command_queue queue;
	cl::Program::Sources sources;
	cl::Program program;
	// cl_program program;
	CLContext(cl::Platform &platform, cl::Device &device);
	void addSource(cl::string source);
	void compileProgram();
	~CLContext();
};
