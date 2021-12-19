__kernel void simple_add(__global int* A, __global int* B, __global int* C)
{
	C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];
};
