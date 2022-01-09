#pragma once

#include "GLObject.h"

class GLRenderer
{
private:

public:
	GLRenderer();
	~GLRenderer();
	void Draw(GLObject &object);
};