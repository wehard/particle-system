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

void CLContext::CheckCLResult(cl_int result, std::string message)
{
	if (result != CL_SUCCESS)
	{
		std::cout << "OpenCL error " << result << " : " << message << std::endl;
		exit(EXIT_FAILURE);
	}
}

CLContext::CLContext()
{
	this->getPlatform();
	this->getDevice();
	this->createContext();
	this->createCommandQueue();
}

void CLContext::addSource(std::string source)
{
	this->source = source;
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
	cl_uint num_devices;

	ctx = clCreateContextFromType(properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &result);
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

void CLContext::compileProgram()
{
	cl_int error = CL_SUCCESS;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;

	program_buffer = (char *)source.c_str();
	program_size = source.size();

	// program_handle = fopen("res/kernel/particles.cl", "r");
	// fseek(program_handle, 0, SEEK_END);
	// program_size = ftell(program_handle);
	// rewind(program_handle);
	// program_buffer = (char*)malloc(program_size + 1); program_buffer[program_size] = '\0'; fread(program_buffer, sizeof(char), program_size, program_handle);
	// fclose(program_handle);

	program = clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer, NULL, &error);
	CheckCLResult(error, "clCreateProgramWithSource");

	error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	CheckCLResult(error, "clBuildProgram");

	std::cout << "Successully compiled kernel program." << std::endl;
}

void CLContext::getDevice()
{
	cl_device_id *devices;
	cl_uint num_devices = 0;

	clGetDeviceIDs(this->platform, CL_DEVICE_TYPE_DEFAULT, 5, NULL, &num_devices);
	devices = (cl_device_id *)malloc(sizeof(cl_device_id) * num_devices);
	clGetDeviceIDs(this->platform, CL_DEVICE_TYPE_DEFAULT, num_devices, devices, NULL);

	if (num_devices == 0)
	{
		std::cout << "CLDevice::CLDevice -- Error: No devices found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available devices:" << std::endl;
	for (int i = 0; i < num_devices; i++)
	{
		char device_name[40];
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
		std::cout << "\t" << device_name << std::endl;
	}

	int dev_index = 1;
	this->device = devices[dev_index];
	clGetDeviceInfo(this->device, CL_DEVICE_NAME, sizeof(clInfo.deviceName), &clInfo.deviceName, NULL);
	std::cout << "Selected device [" << dev_index << "] : " << clInfo.deviceName << std::endl;
}

void CLContext::getPlatform()
{
	cl_int result = CL_SUCCESS;
	cl_platform_id *platforms;
	
	clGetPlatformIDs(5, NULL, &clInfo.numPlatforms);
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * clInfo.numPlatforms);
	clGetPlatformIDs(clInfo.numPlatforms, platforms, NULL);

	if (clInfo.numPlatforms == 0)
	{
		std::cout << "CLPlatform::CLPlatform -- Error: No OpenCL platforms found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available platforms:" << std::endl;

	for (int i = 0; i < clInfo.numPlatforms; i++)
	{
		char pform_vendor[40];

		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pform_vendor), &pform_vendor, NULL);
		std::cout << "\t" << pform_vendor << std::endl;
	}

	this->platform = platforms[0];
	clGetPlatformInfo(this->platform, CL_PLATFORM_VENDOR, sizeof(clInfo.platformVendor), &clInfo.platformVendor, NULL);
	clGetPlatformInfo(this->platform, CL_PLATFORM_VERSION, sizeof(clInfo.platformName), &clInfo.platformName, NULL);
	std::cout << "Selected platform: " << clInfo.platformVendor << ", " << clInfo.platformName << std::endl;
}

CLContext::~CLContext() {}
