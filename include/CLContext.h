/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:21:55 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/06 15:55:15 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 300
#define CL_MINIMUM_OPENCL_VERSION 300

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/opencl.h"
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
	void createContext();
	void createCommandQueue();
	static cl_device_id getDevice(cl_platform_id platform, CLInfo &outInfo, int deviceIndex);
	static cl_platform_id getPlatform(CLInfo &outInfo, int platformIndex);
	
public:
	CLInfo clInfo;
	cl_context ctx;
	cl_command_queue queue;
	CLContext(int platformIndex, int deviceIndex);
	~CLContext();
	void AquireGLObject(cl_mem clMem);
	void ReleaseGLObject(cl_mem clMem);
	static void LogPlatformDeviceInfo();
	static void CheckCLResult(cl_int result, std::string message);
};
