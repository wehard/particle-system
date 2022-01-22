#pragma once

#include "GLObject.h"
#include "Shader.h"
#include "Camera.h"

class GLRenderer
{
private:
	Camera *camera;
public:
	GLRenderer();
	~GLRenderer();
	void Begin(Camera &camera);
	void Draw(GLObject &object);
	void Draw(GLObject &object, Shader &shader);
	void DrawBillboard(GLObject &object, float scale, Shader &shader);
	void DrawLines(GLObject &object, Shader &shader);
};