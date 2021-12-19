/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CLContext.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 17:21:55 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/19 20:52:46 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include "CL/opencl.hpp"

class CLContext
{
private:
	cl::Platform &platform;
	cl::Device &device;
public:
	cl::Context ctx;
	cl::CommandQueue queue;
	cl::Program::Sources sources;
	cl::Program program;
	CLContext(cl::Platform &platform, cl::Device &device);
	void addSource(cl::string source);
	void compileProgram();
	~CLContext();
};
