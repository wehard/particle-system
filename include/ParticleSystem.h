/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 21:18:54 by wkorande          #+#    #+#             */
/*   Updated: 2022/06/20 15:29:51 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLContext.h"
#include "CLContext.h"

#include "CLProgram.h"
#include "GLRenderer.h"

#include "Shader.h"
#include "Camera.h"

#include "Utils.h"

#define MAX_GP 10

typedef enum e_init_shape
{
	CUBE,
	SPHERE,
	TORUS,
	RECT,
	CIRCLE,
	SINE
} t_init_shape;

typedef struct
{
	cl_float4 pos;
	cl_float4 vel;
	cl_float life;
}		t_particle;

typedef struct
{
	cl_float4 pos;
	cl_float4 dir;
	cl_float vel;
	cl_float rate;
	cl_float life;
	cl_float radius;
}		t_emitter;

struct Emitter
{
	glm::vec3 position;
	glm::vec3 direction;
	float velocity;
	float rate;
	float life;
	float radius;

	Emitter()
	{
		position = glm::vec3(0.0, 0.0, 0.0);
		direction = glm::vec3(0.0, 0.0, 0.0);
		velocity = 10000.0;
		rate = 5000.0;
		life = 15.0;
		radius = 0.5f;
	}

	t_emitter CLType()
	{
		t_emitter e;
		e.pos = {position.x, position.y, position.z, 1.0};
		e.dir = {direction.x, direction.y, direction.z, 1.0};
		e.vel = velocity;
		e.rate = rate;
		e.life = life;
		e.radius = radius;
		return e;
	}
};

struct GLBuffer
{
	GLuint vbo;
	GLuint vao;
	cl_mem clmem;
};

struct CLBuffer
{
	cl_mem clmem;
};

class ParticleSystem
{
private:
	GLRenderer renderer;
	Shader &particleShader;
	Shader &emitterShader;
	void CreateParticleBuffer();
	void CreateGravityPointBuffer();
	void CreateSeedBuffer();
	CLProgram &clProgram;
public:
	CLContext &cl;
	GLContext &gl;
	const size_t numParticles = 1000000;
	GLBuffer pBuffer;
	GLBuffer gpBuffer;
	CLBuffer seedBuffer;
	bool useEmitter = false;
	glm::vec4 minColor;
	glm::vec4 maxColor;
	Emitter emitter;
	glm::vec3 position = glm::vec3(0.0f);
	t_init_shape initShape;
	ParticleSystem(GLContext &gl, CLContext &cl, CLProgram &p, Shader &pShader, Shader &eShader);
	void InitParticles();
	void InitParticlesEmitter();
	void UpdateParticles(float deltaTime, std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo);
	void UpdateParticlesEmitter(float deltaTime, std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo);
	void UpdateGpBuffer(std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo);

	void Update(float deltaTime, std::vector<cl_float4> gravityPoints, MouseInfo mouseInfo);
	void Render(Camera &camera, MouseInfo mouseInfo, float particleSize);

	void Reset();
	void SetShape(t_init_shape shape);

	~ParticleSystem();
};

