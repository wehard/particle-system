#include "CLProgram.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <string.h>

CLProgram::CLProgram(CLContext &cl, const char *kernelSource) : cl(cl)
{
	source = loadKernelSource(kernelSource);
	compileProgram();
	createKernels();
}

CLProgram::~CLProgram()
{
	CLContext::CheckCLResult(clReleaseProgram(program), "clReleaseProgram");
}

std::string CLProgram::loadKernelSource(const char *source)
{
	std::ifstream t(source);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

void CLProgram::compileProgram()
{
	cl_int error = CL_SUCCESS;
	FILE *program_handle;

	char *program_buffer, *program_log;
	size_t program_size, log_size;

	program_buffer = (char *)source.c_str();
	program_size = source.size();

	cl_device_id device = cl.GetDeviceID();
	size_t logLength = 0;

	program = clCreateProgramWithSource(cl.ctx, 1, (const char **)&program_buffer, NULL, &error);
	CLContext::CheckCLResult(error, "clCreateProgramWithSource");

	error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	CLContext::CheckCLResult(error, "clBuildProgram");
	if (error != CL_SUCCESS)
	{
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logLength);
		char *buffer = (char *)malloc(sizeof(char) * logLength);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logLength, buffer, NULL);
		std::cout << "OpenCL build error " << buffer << " : " << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Successully created and compiled kernel program." << std::endl;
}

void CLProgram::createKernels()
{
	cl_int result = CL_SUCCESS;
	result = clCreateKernelsInProgram(this->program, 0, NULL, &info.numKernels);
	CLContext::CheckCLResult(result, "clCreateKernelsInProgram");
	std::cout << "Successully created " << info.numKernels << " kernels." << std::endl;

	cl_kernel kernels[info.numKernels];

	result = clCreateKernelsInProgram(this->program, info.numKernels, (cl_kernel *)&kernels, NULL);
	CLContext::CheckCLResult(result, "clCreateKernelsInProgram");

	for (auto k : kernels)
	{
		clKernels.push_back(new CLKernel(program, k));
	}
}

cl_kernel CLProgram::CreateKernel(const char *kernelFunc)
{
	cl_int result = CL_SUCCESS;
	cl_kernel kernel = clCreateKernel(program, kernelFunc, &result);
	CLContext::CheckCLResult(result, "clCreateKernel");
	return kernel;
}

CLProgramInfo CLProgram::GetInfo()
{
	return this->info;
}

CLKernel *CLProgram::GetKernel(const char *kernelName)
{
	for (auto kernel : clKernels)
	{
		if (strcmp(kernelName, kernel->name) == 0)
		{
			return kernel;
		}
	}
	throw;
}
