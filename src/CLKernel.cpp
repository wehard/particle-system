#include "CLKernel.h"
#include "CLContext.h"
#include <iostream>

CLKernel::CLKernel(cl_program program, cl_kernel kernel) : program(program), kernel(kernel)
{
	cl_int result = CL_SUCCESS;

	clGetKernelInfo(kernel, CL_KERNEL_FUNCTION_NAME, sizeof(name), &name, NULL);
	CLContext::CheckCLResult(result, "clGetKernelInfo CL_KERNEL_FUNCTION_NAME");
	std::cout << "Successully created kernel: " << name << std::endl;
}

CLKernel::~CLKernel()
{
	CLContext::CheckCLResult(clReleaseKernel(kernel), "clReleaseKernel");
}

void CLKernel::SetArgs(std::vector<CLKernelArg> args, size_t argc)
{
	cl_int result = CL_SUCCESS;

	for (size_t i = 0; i < argc; i++)
	{
		auto arg = args[i];
		result = clSetKernelArg(kernel, (cl_uint)i, arg.size, arg.arg);
		CLContext::CheckCLResult(result, "CLKernel::SetArgs");
	}
}

void CLKernel::Enqueue(cl_command_queue queue, size_t globalWorkSize)
{
	cl_int result = CL_SUCCESS;
	result = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, NULL, 0, NULL, NULL);
	CLContext::CheckCLResult(result, "CLKernel::Enqueue");
}