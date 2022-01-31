#pragma once

#include "glm/vec3.hpp"
#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/opencl.h"
#include "GL/glx.h"
#endif

struct MouseInfo
{
	glm::vec3 screen;
	glm::vec3 ndc;
	glm::vec3 world;
	bool gravity = false;
	float mass = 1.0f;

	cl_float4 CLTypeWorld()
	{
		cl_float4 m;
		m.s[0] = world.x;
		m.s[1] = world.y;
		m.s[2] = world.z;
		m.s[3] = 1.0f;
		return m;
	}
};