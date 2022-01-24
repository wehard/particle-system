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

#include "CLProgram.h"
#include "GLRenderer.h"

#include "Shader.h"
#include "Camera.h"

#define MAX_GP 10

typedef enum e_init_shape
{
	CUBE,
	SPHERE,
	RECT,
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
	cl_float cone_angle;
}		t_emitter;

struct Emitter
{
	glm::vec3 position;
	glm::vec3 direction;
	float velocity;
	float rate;
	float life;
	float cone_angle;

	Emitter()
	{
		position = glm::vec3(0.0, 0.0, 0.0);
		direction = glm::vec3(0.0, 1.0, 0.0);
		velocity = 10000.0;
		rate = 500.0;
		life = 5.0;
		cone_angle = 30.0f;
	}

	t_emitter CLType()
	{
		t_emitter e;
		e.pos = {position.x, position.y, position.z, 1.0};
		e.dir = {direction.x, direction.y, direction.z, 1.0};
		e.vel = velocity;
		e.rate = rate;
		e.life = life;
		e.cone_angle = cone_angle;
		return e;
	}
};

struct MouseInfo
{
	glm::vec3 screen;
	glm::vec3 ndc;
	glm::vec3 world;

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
	Shader *particleShader;
	Shader *basicShader;
	Shader *vertexColorShader;
	Shader *billboardShader;
	void CreateParticleBuffer();
	void CreateGravityPointBuffer();
	void CreateSeedBuffer();
	double lastTime = 0.0;
public:
	int fps;
	double deltaTime;
	int mouseGravity = 0;
	float mouseGravityScale = 1.0f;
	std::vector<cl_float4> gravityPoints;
	MouseInfo mouseInfo;
	CLProgram *clProgram;
	CLContext &cl;
	GLContext &gl;
	const size_t numParticles = 2000000;
	float particleSize = 1.0f;
	GLBuffer pBuffer;
	GLBuffer gpBuffer;
	CLBuffer seedBuffer;
	bool showOverlays = true;
	bool useEmitter = false;
	glm::vec4 minColor;
	glm::vec4 maxColor;
	Camera camera;
	Emitter emitter;
	bool mouseMovement = false;
	ParticleSystem(GLContext &gl, CLContext &cl);
	void InitParticles(t_init_shape shape);
	void InitParticlesEmitter();
	void UpdateParticles(float deltaTime);
	void UpdateParticlesEmitter(float deltaTime);
	void UpdateGpBuffer();

	void Update();
	void Render();

	void Reset();
	void AddGravityPoint();

	void Run();
	~ParticleSystem();
};

