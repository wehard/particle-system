#include "GUIContext.h"
#include <string>
#include <algorithm>

GUIContext::GUIContext() {}

void GUIContext::Init(GLFWwindow *window, const char *glslVersion)
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);
}

void GUIContext::Update(Application &app)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	UpdatePlatformInfo(app);
	UpdateApplicationInfo(app);
	UpdateSystems(app);
}

void GUIContext::UpdateApplicationInfo(Application &app)
{
	ImGui::Begin("application");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("FPS %d", app.fps);
	ImGui::Text("ms %f", app.msPerFrame);
	ImGui::Checkbox("Show Overlays", &app.showOverlays);
	ImGui::Separator();

	ImGui::Text("Camera");
	ImGui::Text("Position x %f, y %f, z %f", app.camera.position.x, app.camera.position.y, app.camera.position.z);
	ImGui::Text("Yaw %f, Pitch %f", app.camera.yaw, app.camera.pitch);
	if (ImGui::SmallButton("TopLeft"))
	{
		app.camera.Reset(glm::vec3(-1.0, 2.0, 2.0), -65.0f, -40.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Front"))
	{
		app.camera.Reset(glm::vec3(0.0, 0.0, 2.0), -90.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Back"))
	{
		app.camera.Reset(glm::vec3(0.0, 0.0, -2.0), 90.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Left"))
	{
		app.camera.Reset(glm::vec3(-2.0, 0.0, 0.0), 0.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Right"))
	{
		app.camera.Reset(glm::vec3(2.0, 0.0, 0.0), 180.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Top"))
	{
		app.camera.Reset(glm::vec3(0.0, 2.0, 0.0), -90.0f, -89.0f);
	}
	ImGui::Separator();

	ImGui::Text("Mouse");
	ImGui::Text("Screen x %f, y %f, z %f", app.mouseInfo.screen.x, app.mouseInfo.screen.y, app.mouseInfo.screen.z);
	ImGui::Text("NDC x %f, y %f, z %f", app.mouseInfo.ndc.x, app.mouseInfo.ndc.y, app.mouseInfo.ndc.z);
	ImGui::Text("World x %f, y %f, z %f", app.mouseInfo.world.x, app.mouseInfo.world.y, app.mouseInfo.world.z);
	ImGui::Checkbox("Gravity", (bool *)&app.mouseInfo.gravity);
	ImGui::SameLine();
	ImGui::SliderFloat("Mass", &app.mouseInfo.mass, 1.0, 100.0, "%.2f", ImGuiSliderFlags_None);
	ImGui::Separator();

	ImGui::Text("Info");
	ImGui::Text("Count %zu", app.GetNumParticles());
	ImGui::SliderFloat("Size", &app.particleSize, 1.0f, 5.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	ImGui::ColorEdit3("Background color", &app.gl.clearColor.r);

	ImGui::Text("Gravity points: %lu", app.gravityPoints.size());
	if (ImGui::Button("Clear", ImVec2(50, 20)))
		app.gravityPoints.clear();

	for (size_t i = 0; i < app.gravityPoints.size(); i++)
	{
		cl_float4 p = app.gravityPoints[i];
		float v[4] = {p.s[0], p.s[1], p.s[2], p.s[3]};
		if (ImGui::DragFloat4(std::to_string(i).c_str(), v, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			app.gravityPoints[i].s[0] = v[0];
			app.gravityPoints[i].s[1] = v[1];
			app.gravityPoints[i].s[2] = v[2];
			app.gravityPoints[i].s[3] = v[3];
		}
	}
	ImGui::End();
}

void GUIContext::UpdatePlatformInfo(Application &app)
{
	ImGui::Begin("platform");
	ImGui::Text("GL version: %d.%d", app.gl.glInfo.glMajorVersion, app.gl.glInfo.glMinorVersion);
	ImGui::Text("GLSL version: %s", app.gl.glInfo.shadingLanguageVersion);
	ImGui::Text("GL Vendor: %s", app.gl.glInfo.vendor);
	ImGui::Text("GL Renderer: %s", app.gl.glInfo.renderer);
	ImGui::Separator();
	ImGui::Text("CL Platform: %s, %s", app.cl.clInfo.platformVendor, app.cl.clInfo.platformName);
	ImGui::Text("CL Device: %s", app.cl.clInfo.deviceName);
	ImGui::Separator();
	ImGui::Text("CL Program");
	for (auto k : app.clProgram->clKernels)
	{
		ImGui::Text("%s", k->name);
	}

	ImGui::End();
}

void GUIContext::UpdateSystems(Application &app)
{
	ImGui::Begin("particle systems");
	if (ImGui::Button("Add"))
	{
		app.AddParticleSystem();
	}
	ImGui::Separator();

	for (auto ps : app.particleSystems)
	{
		ImGui::PushID(ps);
		ImGui::Text("Particle System");
		ImGui::SameLine();
		if (ImGui::SmallButton("Remove"))
		{
			app.particleSystems.erase(std::remove(app.particleSystems.begin(), app.particleSystems.end(), ps), app.particleSystems.end());
		}
		if (ImGui::SmallButton("Sphere"))
		{
			ps->SetShape(SPHERE);
			ps->Reset();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Torus"))
		{
			app.ClearGravityPoints();
			app.AddGravityPoint(glm::vec4(0.0, 0.0, 0.0, 0.5));
			app.AddGravityPoint(glm::vec4(-0.25, 0.0, 0.0, 0.0017));
			app.AddGravityPoint(glm::vec4(-0.45, 0.0, 0.0, 0.0087));
			ps->SetShape(TORUS);
			ps->Reset();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Cube"))
		{
			ps->SetShape(CUBE);
			ps->Reset();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Rect"))
		{
			ps->SetShape(RECT);
			ps->Reset();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Circle"))
		{
			app.ClearGravityPoints();
			app.AddGravityPoint(glm::vec4(0.0, 0.0, 0.0, 3.0));
			ps->SetShape(CIRCLE);
			ps->Reset();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Sine"))
		{
			ps->SetShape(SINE);
			ps->Reset();
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Emitter"))
		{
			ps->useEmitter = true;
			ps->InitParticlesEmitter();
		}
		ImGui::ColorEdit4("Min color", &ps->minColor.x);
		ImGui::ColorEdit4("Max color", &ps->maxColor.x);

		float v[3] = {ps->emitter.position.x, ps->emitter.position.y, ps->emitter.position.z};
		if (ImGui::DragFloat3("Position", v, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps->emitter.position = glm::vec3(v[0], v[1], v[2]);
			ps->position = glm::vec3(v[0], v[1], v[2]);
			if (ps->useEmitter)
			{
				ps->InitParticlesEmitter();
			}
			else
			{
				ps->InitParticles();
			}
		}

		if (ps->useEmitter)
		{
			float v2[3] = {ps->emitter.direction.x, ps->emitter.direction.y, ps->emitter.direction.z};
			if (ImGui::DragFloat3("Rotation", v2, 0.1, -180.0, 180.0, "%.3f", ImGuiSliderFlags_None))
			{
				ps->emitter.direction = glm::vec3(v2[0], v2[1], v2[2]);
				ps->InitParticlesEmitter();
			}
			if (ImGui::SliderFloat("Radius", &ps->emitter.radius, 0.01f, 1.0f, "%.2f", ImGuiSliderFlags_None))
				ps->InitParticlesEmitter();
			if (ImGui::SliderFloat("Rate", &ps->emitter.rate, 1, 50000))
				ps->InitParticlesEmitter();
			if (ImGui::SliderFloat("Life", &ps->emitter.life, 0.1, 20.0, "%.2f", ImGuiSliderFlags_None))
				ps->InitParticlesEmitter();
			if (ImGui::SliderFloat("Speed", &ps->emitter.velocity, 1000, 20000, "%f", ImGuiSliderFlags_None))
				ps->InitParticlesEmitter();
		}
		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::End();
}

void GUIContext::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIContext::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(context);
}

GUIContext::~GUIContext() {}
