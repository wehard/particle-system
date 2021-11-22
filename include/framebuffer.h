/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/13 18:19:52 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 19:04:47 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace wgl
{

class Texture;
class TexturedQuad;
class Shader;

class Framebuffer
{
private:
	unsigned int fbo_id;
	unsigned int rbo_id;
	int width;
	int height;
	Texture *texture;
	TexturedQuad *quad;
	Shader *shader;

public:
	// unsigned int tex_id;
	Framebuffer(int width, int height);
	~Framebuffer();
	void Bind();
	void Unbind();
	Texture *getTexture();
	void draw();
};

} // namespace wgl