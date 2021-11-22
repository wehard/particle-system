/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wgl_input.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/30 13:30:30 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 17:21:56 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "input.h"
#include <algorithm>
#include <iostream>

namespace wgl
{

std::vector<Input *> Input::input_instances;

Input::Input(std::vector<int> keys_to_monitor) : is_enabled(true), keys()
{
	input_instances.push_back(this);
	for (int key : keys_to_monitor)
		keys[key] = false;
	// std::cout << "constructor" << std::endl;
}

Input::Input() : is_enabled(true), keys()
{
	std::cout << "default" << std::endl;
	input_instances.push_back(this);
}

Input::~Input()
{
	Input::input_instances.erase(std::remove(Input::input_instances.begin(), Input::input_instances.end(), this), Input::input_instances.end());
}

void Input::setupKeyInputs(GLFWwindow *window)
{
	glfwSetKeyCallback(window, glfwKeyCallback);
}

void Input::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_REPEAT)
		return;
	for (Input *input : input_instances)
	{
		input->setIsKeyDown(key, action != GLFW_RELEASE);
	}
}

void Input::setIsKeyDown(int key, bool is_down)
{
	if (!is_enabled)
	{
		std::cout << "not enabled! " << key << std::endl;
		return;
	}
	std::map<int, bool>::iterator it = keys.find(key);
	if (it != keys.end())
	{
		keys[key] = is_down;
	}
}

bool Input::isKeyDown(int key)
{
	bool result = false;
	if (is_enabled)
	{
		std::map<int, bool>::iterator it = keys.find(key);
		if (it != keys.end())
		{
			result = keys[key];
		}
	}
	return (result);
}

} // namespace wgl
