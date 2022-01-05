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
	cl_uint numDevices;
	cl_uint numPlatforms;
};

class CLContext
{
private:
	cl_platform_id platform;
	cl_device_id device;
	cl_int numDevices;
	void getDevice();
	void getPlatform();
	void createContext();
	void createCommandQueue();
	
public:
	CLInfo clInfo;
	cl_context ctx;
	cl_command_queue queue;
	CLContext();
	~CLContext();
	static void CheckCLResult(cl_int result, std::string message);
};
