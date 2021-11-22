/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/30 19:25:13 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 20:53:57 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj_loader.h"
#include "mesh.h"
#include <stdio.h>
#include <string.h>
#include <string>

namespace wgl
{

Mesh *loadObj(std::string path)
{
	Mesh *m = new Mesh();
	m->name = path;
	// std::vector<glm::vec3> out_verts;
	// std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> temp_normals;
	// std::vector<int> out_indices;

	FILE *f = fopen(path.c_str(), "r");
	if (!f)
	{
		printf("error opening file: %s\n", path.c_str());
		return (NULL);
	}
	char buf[128];
	while (true)
	{
		int res = fscanf(f, "%s", buf);
		if (res == EOF)
			break;

		if (strcmp(buf, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(f, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			m->vertices.push_back(vertex.x);
			m->vertices.push_back(vertex.y);
			m->vertices.push_back(vertex.z);

			m->normals.push_back(1.0);
			m->normals.push_back(1.0);
			m->normals.push_back(1.0);

			m->colors.push_back(1.0);
			m->colors.push_back(1.0);
			m->colors.push_back(1.0);
			m->colors.push_back(1.0);
		}
		else if (strcmp(buf, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(f, "%f %f\n", &uv.x, &uv.y);
			m->uvs.push_back(uv.x);
			m->uvs.push_back(uv.y);
		}
		else if (strcmp(buf, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(f, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
			// m->normals.push_back(normal.x);
			// m->normals.push_back(normal.y);
			// m->normals.push_back(normal.z);
		}
		else if (strcmp(buf, "f") == 0)
		{
			int v_i[3];
			int t_i[3];
			int n_i[3];
			fscanf(f, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v_i[0], &t_i[0], &n_i[0], &v_i[1], &t_i[1], &n_i[1], &v_i[2], &t_i[2], &n_i[2]);
			for (int i = 0; i < 3; i++)
			{
				v_i[i] -= 1;
				t_i[i] -= 1;
				n_i[i] -= 1;
			}
			m->indices.push_back(v_i[0]);
			m->indices.push_back(v_i[1]);
			m->indices.push_back(v_i[2]);

			m->normals[(v_i[0] * 3)] = temp_normals[n_i[0]].x;
			m->normals[(v_i[0] * 3) + 1] = temp_normals[n_i[0]].y;
			m->normals[(v_i[0] * 3) + 2] = temp_normals[n_i[0]].z;

			m->normals[(v_i[1] * 3)] = temp_normals[n_i[1]].x;
			m->normals[(v_i[1] * 3) + 1] = temp_normals[n_i[1]].y;
			m->normals[(v_i[1] * 3) + 2] = temp_normals[n_i[1]].z;

			m->normals[(v_i[2] * 3)] = temp_normals[n_i[2]].x;
			m->normals[(v_i[2] * 3) + 1] = temp_normals[n_i[2]].y;
			m->normals[(v_i[2] * 3) + 2] = temp_normals[n_i[2]].z;
		}
	}
	return (m);
}

std::vector<float> convertVec3s(std::vector<glm::vec3> v3s)
{
	std::vector<float> fs;
	for (auto v : v3s)
	{
		fs.push_back(v.x);
		fs.push_back(v.y);
		fs.push_back(v.z);
	}
	return (fs);
}

} // namespace wgl
