#pragma once

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/opencl.hpp"
#include "GL/glx.h"
#endif

#include <string>
#include "CLContext.h"

class CLProgram
{
private:
	std::string source;
	cl_program program;
	std::string loadKernelSource(const char *source);
	void compileProgram();
	CLContext &cl;
public:
	CLProgram(CLContext &cl, const char *kernelSource);
	~CLProgram();
	cl_kernel CreateKernel(const char *kernelFunc);
};


