#pragma once

#include "GLContext.h"
#include "CLContext.h"
#include "CLProgram.h"
#include "Shader.h"
#include "Camera.h"

#include "Utils.h"

class Application
{
private:
	Shader *particleShader;
	Shader *emitterShader;
	Shader *basicShader;
	Shader *vertexColorShader;
	Shader *billboardShader;



	float lastTime;
	float deltaTime;


public:
	float angle = 0.0f;
	std::vector<ParticleSystem *> particleSystems;
	CLProgram *clProgram;
	bool showOverlays = true;
	std::vector<cl_float4> gravityPoints;
	size_t numParticles;
	float particleSize = 1.0f;
	int fps;
	CLContext &cl;
	GLContext &gl;
	bool mouseMovement = false;
	Camera camera;
	MouseInfo mouseInfo;
	void ClearGravityPoints();
	void AddGravityPoint();
	void AddGravityPoint(glm::vec4 pos);
	size_t GetNumParticles();
	Application(GLContext &gl, CLContext &cl);
	~Application();
	void Run();
	void AddParticleSystem();
};


