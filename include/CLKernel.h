#pragma once

#include "CLContext.h"
#include <vector>

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
	cl_program program;
	CLKernel(cl_program program, cl_kernel kernel);
	~CLKernel();
	void SetArgs(std::vector<CLKernelArg> args, size_t argc);
	void Enqueue(cl_command_queue queue, size_t globalWorkSize);
};


