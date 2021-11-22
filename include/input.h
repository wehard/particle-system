/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wgl_input.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/30 13:05:54 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 17:21:32 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

namespace wgl
{

class Input
{
private:
	std::map<int, bool> keys;
	bool is_enabled;

public:
	static std::vector<Input *> input_instances;

	Input(std::vector<int> keys);
	Input();
	~Input();
	bool getIsEnabled() { return is_enabled; }
	void setIsEnabled(bool value) { is_enabled = value; }

	static void setupKeyInputs(GLFWwindow *window);
	static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	void setIsKeyDown(int key, bool is_down);
	bool isKeyDown(int key);
};

} // namespace wgl
