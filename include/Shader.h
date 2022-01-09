#pragma once

#include <iostream>
#include <map>
#include <glm/glm.hpp>

class Shader
{
private:
	uint32_t p_id;
	uint32_t v_id;
	uint32_t f_id;
	std::string vert_path;
	std::string frag_path;
	std::map<std::string, uint32_t> uniforms;
	std::map<std::string, uint32_t> attributes;

	std::string loadShader(std::string path);
	uint32_t compileShader(std::string src, uint32_t shader_type);
	uint32_t createProgram(uint32_t vert_id, uint32_t frag_id);
	void loadUniforms();
	void loadAttributes();

public:
	Shader(std::string vert_path, std::string frag_path);
	~Shader();
	void use();
	void setFloat(std::string name, float f);
	void setVec2(std::string name, glm::vec2 v);
	void setVec3(std::string name, glm::vec3 v);
	void setVec4(std::string name, glm::vec4 v);
	void setMat4(std::string name, glm::mat4x4 m);
};
