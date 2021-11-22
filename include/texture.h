#pragma once

#include <string>

namespace wgl
{

class Texture
{
private:
	unsigned int tex_id;
public:
	int width;
	int height;
	Texture(int width, int height);
	Texture(std::string path);
	~Texture();
	unsigned int getTextureID();
};

}