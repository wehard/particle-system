/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 19:41:00 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/05 00:19:40 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gl-engine.h"

#include <stdlib.h>
#include "CLContext.h"
#include "GLContext.h"
#include "ParticleSystem.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

static cl_platform_id getPlatform()
{
	cl_int result = CL_SUCCESS;
	cl_platform_id *platforms;
    cl_uint num_platforms = 0;
	
	clGetPlatformIDs(5, NULL, &num_platforms);
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);
	clGetPlatformIDs(num_platforms, platforms, NULL);

	if (num_platforms == 0)
	{
		std::cout << "CLPlatform::CLPlatform -- Error: No OpenCL platforms found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Available platforms:" << std::endl;

	for (int i = 0; i < num_platforms; i++)
	{
		char pform_vendor[40];

		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pform_vendor), &pform_vendor, NULL);
		std::cout << "\t" << pform_vendor << std::endl;
	}

	cl_platform_id platform = platforms[0];
	char pform_vendor[40];
	char pform_name[40];
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(pform_vendor), &pform_vendor, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(pform_name), &pform_name, NULL);
	std::cout << "Selected platform: " << pform_vendor << ", " << pform_name << std::endl;
	return platform;
}

static cl_device_id getDevice(cl_platform_id platform)
{
	cl_device_id *devices;
	cl_uint num_devices = 0;

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 5, NULL, &num_devices);
	devices = (cl_device_id*)malloc(sizeof(cl_device_id) * num_devices);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, num_devices, devices, NULL);

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
	cl_device_id device = devices[dev_index];
	char device_name[40];
	clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
	std::cout << "Selected device [" << dev_index << "] : " << device_name << std::endl;
	return device;
}

int main(void)
{
	GLContext gl = GLContext("particle-system", 1280, 720);
	
	cl_platform_id platform = getPlatform();
	cl_device_id device = getDevice(platform);
	CLContext cl = CLContext(platform, device);
	

	ParticleSystem ps = ParticleSystem(gl, cl);
	ps.init();
	gl.run(&ps);

	return (0);
}
