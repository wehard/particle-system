/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 19:41:00 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/06 15:53:29 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 300
#define CL_MINIMUM_OPENCL_VERSION 300

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "gl-engine.h"

#include <stdlib.h>
#include "CLContext.h"
#include "GLContext.h"
#include "ParticleSystem.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage: particle-system [device]\n\n");
		CLContext::LogPlatformDeviceInfo();
		return (1);
	}

	std::istringstream iss(argv[1]);
	int deviceIndex;

	if (iss >> deviceIndex)
	{
		CLContext cl = CLContext(0, deviceIndex);
		GLContext gl = GLContext("particle-system", 1280, 720);

		ParticleSystem ps = ParticleSystem(gl, cl);
		ps.init("init_particles_sphere");
		gl.run(&ps);
	}

	return (0);
}
