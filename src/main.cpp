/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 19:41:00 by wkorande          #+#    #+#             */
/*   Updated: 2021/11/22 20:59:14 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wgl.h"

class App : public wgl::Application
{
	private:
	wgl::Shader *s;
	wgl::Mesh *m;
	wgl::Entity *e;

	public:
	App(std::string title) : wgl::Application(title, 1280, 720) {}

	virtual void onAttach() override
	{
		s = new wgl::Shader("./res/shaders/phong.vert", "./res/shaders/phong.frag");
		m = wgl::loadObj("./res/obj/cube.obj");
		m->setVertexColors(glm::vec4(0.0, 1.0, 1.0, 1.0));
		
		e = new wgl::Entity(s, m);
		e->position = glm::vec3(0.0, 0.0, 0.0);
		e->scale = glm::vec3(10.0f, 10.0f, 10.0f);
		addEntity(e);

		camera->position = glm::vec3(0.0, 0.0, 20.0);
		
	}

	virtual void onUpdate(float deltaTime) override
	{

	}

	virtual void onDetach() override
	{
		delete m;
		delete s;
		delete e;
	}
};


int main(void)
{
	App *app = new App("particle-system");
	app->run();
	delete app;
	return (0);
}
