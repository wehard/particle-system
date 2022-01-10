/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:18:47 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/06 16:10:39 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CLContext.h"
#include <iostream>
#ifdef __linux__
#include "CL/opencl.h"
#include "GL/glx.h"
#endif

#if defined(__APPLE__)
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/OpenGL.h>
#endif

void CLContext::CheckCLResult(cl_int result, std::string message)
{
	if (result != CL_SUCCESS)
	{
		std::cout << "OpenCL error " << result << " : " << message << std::endl;
		exit(EXIT_FAILURE);
	}
}

CLContext::CLContext(int platformIndex, int deviceIndex)
{
	platform = this->getPlatform(this->clInfo, platformIndex);
	device = this->getDevice(this->platform, this->clInfo, deviceIndex);
	this->createContext();
	this->createCommandQueue();
}

void CLContext::createContext()
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

	ctx = clCreateContextFromType(properties, CL_DEVICE_TYPE_ALL, NULL, NULL, &result);
	CheckCLResult(result, "clCreateContext");
	std::cout << "Successully created CL context." << std::endl;

	result = clGetContextInfo(ctx, CL_CONTEXT_NUM_DEVICES, sizeof(clInfo.numDevices), &clInfo.numDevices, NULL);
	CheckCLResult(result, "clGetContextInfo");
	printf("\tNum devices: %d\n", clInfo.numDevices);
}

void CLContext::createCommandQueue()
{
	cl_int result;
	queue = clCreateCommandQueue(this->ctx, device, 0, &result);
	CheckCLResult(result, "clCreateCommandQueue");

	std::cout << "Successully created CL command queue." << std::endl;
}

cl_platform_id CLContext::getPlatform(CLInfo &outInfo, int plaformIndex)
{
	cl_int result = CL_SUCCESS;
	cl_platform_id *platforms;
	
	clGetPlatformIDs(5, NULL, &outInfo.numPlatforms);
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * outInfo.numPlatforms);
	clGetPlatformIDs(outInfo.numPlatforms, platforms, NULL);

	if (outInfo.numPlatforms == 0)
	{
		std::cout << "CLPlatform::CLPlatform -- Error: No OpenCL platforms found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available platforms:" << std::endl;

	for (int i = 0; i < outInfo.numPlatforms; i++)
	{
		char pform_vendor[40];

		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pform_vendor), &pform_vendor, NULL);
		std::cout << "\t" << pform_vendor << std::endl;
	}

	if (plaformIndex < 0)
	{
		return NULL;
	}

	cl_platform_id platform = platforms[0];
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(clInfo.platformVendor), &outInfo.platformVendor, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(clInfo.platformName), &outInfo.platformName, NULL);
	std::cout << "Selected platform: " << outInfo.platformVendor << ", " << outInfo.platformName << std::endl;
	free(platforms);
	return platform;
}

cl_device_id CLContext::getDevice(cl_platform_id platform, CLInfo &outInfo, int deviceIndex)
{
	cl_device_id *devices;

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 5, NULL, &outInfo.numDevices);
	devices = (cl_device_id *)malloc(sizeof(cl_device_id) * outInfo.numDevices);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, outInfo.numDevices, devices, NULL);

	if (outInfo.numDevices == 0)
	{
		std::cout << "CLDevice::CLDevice -- Error: No devices found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available devices:" << std::endl;
	for (int i = 0; i < outInfo.numDevices; i++)
	{
		char device_name[64];
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
		std::cout << "\t" << i << ": " << device_name << std::endl;
	}

	if (deviceIndex < 0)
	{
		free(devices);
		return NULL;
	}

	cl_device_id device = devices[deviceIndex];
	clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(outInfo.deviceName), &outInfo.deviceName, NULL);
	std::cout << "Selected device [" << deviceIndex << "] : " << outInfo.deviceName << std::endl;
	free(devices);
	return device;
}

CLContext::~CLContext() {}

void CLContext::AquireGLObject(cl_mem clMem)
{
	cl_int result = CL_SUCCESS;
	result = clEnqueueAcquireGLObjects(queue, 1, &clMem, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueAcquireGLObjects");
}

void CLContext::ReleaseGLObject(cl_mem clMem)
{
	cl_int result = CL_SUCCESS;
	result = clEnqueueReleaseGLObjects(queue, 1, &clMem, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "clEnqueueReleaseGLObjects");
}

void CLContext::LogPlatformDeviceInfo()
{
	CLInfo info;

	cl_platform_id platform = CLContext::getPlatform(info, 0);
	cl_device_id device = CLContext::getDevice(platform, info, -1);
}
