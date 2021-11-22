/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 18:22:25 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 19:08:01 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "framebuffer.h"
#include "GL/glew.h"
#include <iostream>
#include "texture.h"
#include "textured_quad.h"
#include "shader.h"

namespace wgl
{

Framebuffer::Framebuffer(int width, int height) : width(width), height(height)
{
	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	// create texture to render to
	shader = new Shader("resources/shaders/texture.vert", "resources/shaders/texture.frag");
	texture = new Texture(width, height);
	quad = new TexturedQuad(texture);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTextureID(), 0);

	glGenRenderbuffers(1, &rbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	else
		std::cout << "Framebuffer created." << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &fbo_id);
	// glDeleteTextures(1, &tex_id);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture *Framebuffer::getTexture()
{
	return (texture);
}

void Framebuffer::draw()
{
	shader->use();
	shader->setVec2("screen_size", glm::vec2((float)width, (float)height));
	quad->draw();
}

}