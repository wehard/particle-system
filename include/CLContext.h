/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:21:55 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/05 00:16:23 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 120

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/opencl.hpp"
#include "GL/glx.h"
#endif

#include <string>
#include <vector>

struct CLInfo
{
	char platformVendor[64];
	char platformName[64];
	char deviceName[64];
};
class CLContext
{
private:
	cl_platform_id platform;
	cl_device_id device;
	cl_int numDevices;
	CLInfo clInfo;
	void getDevice();
	void getPlatform();
public:
	cl_context ctx;
	cl_command_queue queue;
	std::string source;
	cl_program program;
	CLContext();
	void addSource(std::string source);
	void compileProgram();
	~CLContext();
};
