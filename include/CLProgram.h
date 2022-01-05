#pragma once

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/opencl.hpp"
#include "GL/glx.h"
#endif

#include <string>
#include "CLContext.h"
#include "CLKernel.h"
#include <vector>

struct CLProgramInfo
{
	cl_uint numKernels;
};

class CLProgram
{
private:
	CLProgramInfo info;
	cl_uint numKernels;
	CLContext &cl;
	std::string source;
	cl_program program;
	std::string loadKernelSource(const char *source);
	void compileProgram();
	void createKernels();
public:
	std::vector<CLKernel*> clKernels;
	CLProgram(CLContext &cl, const char *kernelSource);
	~CLProgram();
	cl_kernel CreateKernel(const char *kernelFunc);
	// void SetKernelArgs(const char *kernelName, std::vector<CLKernelArg> args);
	// void RunKernel(const char *kernelName, size_t globalWorkSize);
	CLKernel *GetKernel(const char *kernelName);
	CLProgramInfo GetInfo();
};


