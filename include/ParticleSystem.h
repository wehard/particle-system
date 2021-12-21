/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/21 21:17:16 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "shader.h"

class ParticleSystem
{
private:
	cl_mem	clmem;
	CLContext &cl;
	GLContext &gl;
public:
	const int numParticles = 10000;
	GLuint vao;
	GLuint vbo;
	glengine::Shader *shader;
	ParticleSystem(GLContext &gl, CLContext &cl);
	void init();
	void update(float deltaTime);
	~ParticleSystem();
};

