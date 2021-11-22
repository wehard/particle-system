/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/31 10:53:38 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 20:29:43 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core.h"
#include <GL/gl.h>
#include <vector>
#include <glm/vec4.hpp>
#include <string>

namespace wgl
{

struct Bounds;

class Mesh
{
private:
public:
	std::string name;
	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> colors;
	std::vector<GLfloat> uvs;
	std::vector<GLfloat> normals;
	size_t getNumVertices();
	Mesh();
	~Mesh();
	void setVertexColors(glm::vec4 c);
	static Mesh *makeQuad();
	static Bounds calculateBounds(Mesh *mesh);
	static void print(Mesh *m);
	void clear();
};

} // namespace wgl
