#pragma once

#include "CLContext.h"
#include "CLKernel.h"
#include <string>
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
	CLKernel *GetKernel(const char *kernelName);
	CLProgramInfo GetInfo();
};


