#include "CLKernel.h"
#include "CLContext.h"
#include <iostream>

CLKernel::CLKernel(cl_kernel kernel) : kernel(kernel)
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