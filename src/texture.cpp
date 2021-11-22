
#include "texture.h"
#include "GL/glew.h"
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace wgl
{

Texture::Texture(int width, int height) : width(width), height(height)
{
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(std::string path)
{
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture %s!\n", path.c_str());
	}
	printf("texture size: %d %d\n", width, height);
	stbi_image_free(data);
}

Texture::~Texture()
{
	printf("destroyed texture: %d\n", tex_id);
	glDeleteTextures(1, &tex_id);
}

unsigned int Texture::getTextureID()
{
	return (tex_id);
}

}