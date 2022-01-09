/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2021/12/31 21:46:18 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "shader.h"

#include "CLProgram.h"

#define MAX_GP 10

typedef struct
{
	cl_float4 pos;
	cl_float4 vel;
	cl_float life;
}		t_particle;

struct Emitter
{
	cl_float4 position;
	cl_float4 direction;
};

struct MouseInfo
{
	glm::vec3 screen;
	glm::vec3 ndc;
	glm::vec3 world;
};

struct GLBuffer
{
	GLuint vbo;
	GLuint vao;
};

class ParticleSystem
{
private:
	cl_mem	clmem;
	cl_mem	clmemgp;
	void createParticleBuffer();
	void createGravityPointBuffer();
public:
	int mouseGravity = 0;
	std::vector<cl_float4> gravityPoints;
	MouseInfo mouseInfo;
	glm::vec3 rotation = glm::vec3(0.0);
	CLProgram *clProgram;
	CLContext &clCtx;
	GLContext &glCtx;
	const size_t numParticles = 2000000;
	// GLuint vao;
	// GLuint vbo;
	// GLuint gpvao;
	// GLuint gpvbo;
	GLBuffer pBuffer;
	GLBuffer gpBuffer;
	bool renderGravityPoints = true;
	glm::vec4 minColor;
	glm::vec4 maxColor;
	glengine::Shader *shader;
	ParticleSystem(GLContext &gl, CLContext &cl);
	void init(const char *initKernel);
	void update(float deltaTime);
	void updateGp(float deltaTime);
	void updateGpBuffer();
	void reset();
	void AddGravityPoint();
	~ParticleSystem();
};

