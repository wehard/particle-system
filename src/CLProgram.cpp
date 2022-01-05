#include "CLProgram.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

CLProgram::CLProgram(CLContext &cl, const char *kernelSource) : cl(cl)
{
	source = loadKernelSource(kernelSource);
	compileProgram();
}

CLProgram::~CLProgram()
{
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

	// program_handle = fopen("res/kernel/particles.cl", "r");
	// fseek(program_handle, 0, SEEK_END);
	// program_size = ftell(program_handle);
	// rewind(program_handle);
	// program_buffer = (char*)malloc(program_size + 1); program_buffer[program_size] = '\0'; fread(program_buffer, sizeof(char), program_size, program_handle);
	// fclose(program_handle);

	program = clCreateProgramWithSource(cl.ctx, 1, (const char **)&program_buffer, NULL, &error);
	CLContext::CheckCLResult(error, "clCreateProgramWithSource");

	error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	CLContext::CheckCLResult(error, "clBuildProgram");

	std::cout << "Successully created and compled kernel program." << std::endl;
}

cl_kernel CLProgram::CreateKernel(const char *kernelFunc)
{
	cl_int result = CL_SUCCESS;
	cl_kernel kernel = clCreateKernel(program, kernelFunc, &result);
	CLContext::CheckCLResult(result, "clCreateKernel");
	return kernel;
}