/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/19 23:26:59 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "shader.h"

class ParticleSystem
{
private:
	const int numParticles = 100;
	GLuint vao;
	GLuint vbo;
	std::vector<cl::Memory> clMem;
	glengine::Shader *shader;
	CLContext &cl;
	GLContext &gl;

public:
	ParticleSystem(GLContext &gl, CLContext &cl);
	void init();
	void update(float deltaTime);
	~ParticleSystem();
};

