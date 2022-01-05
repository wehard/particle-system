#pragma once

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/opencl.hpp"
#endif

struct CLKernelArg
{
	size_t size;
	const void *arg;
};

class CLKernel
{
private:
public:
	char name[64];
	cl_uint numArgs;
	cl_kernel kernel;
	CLKernel(cl_kernel kernel);
	~CLKernel();
};


