#include "GUIContext.h"

#include <string>

GUIContext::GUIContext() {}

void GUIContext::Init(GLFWwindow *window, const char *glslVersion)
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);
}

void GUIContext::Update(ParticleSystem &ps)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("particle-system");
	ImGui::Text("FPS %d", ps.glCtx.fps);
	ImGui::Text("Particles %zu", ps.numParticles);
	ImGui::Separator();
	ImGui::Text("Camera x %f, y %f, z %f", ps.glCtx.camera->position.x, ps.glCtx.camera->position.y, ps.glCtx.camera->position.z);
	ImGui::Separator();
	ImGui::Text("Mouse");
	ImGui::Text("Screen x %f, y %f, z %f", ps.mouseInfo.screen.x, ps.mouseInfo.screen.y, ps.mouseInfo.screen.z);
	ImGui::Text("NDC x %f, y %f, z %f", ps.mouseInfo.ndc.x, ps.mouseInfo.ndc.y, ps.mouseInfo.ndc.z);
	ImGui::Text("World x %f, y %f, z %f", ps.mouseInfo.world.x, ps.mouseInfo.world.y, ps.mouseInfo.world.z);
	ImGui::Separator();

	if (ImGui::Button("Reset", ImVec2(50, 20)))
		ps.rotation = glm::vec3(0.0);
	float v[3] = {ps.rotation.x, ps.rotation.y, ps.rotation.z};
	if (ImGui::DragFloat3("Rotation", v, 0.5, -360.0, 360.0, "%.2f", ImGuiSliderFlags_None))
	{
		ps.rotation = glm::vec3(v[0], v[1], v[2]);
	}
	ImGui::ColorEdit3("Min color", &ps.minColor.x);
	ImGui::ColorEdit3("Max color", &ps.maxColor.x);
	ImGui::Separator();
	ImGui::Text("Reset");
	if (ImGui::Button("Sphere", ImVec2(50, 20)))
		ps.init("init_particles_sphere");
	if (ImGui::Button("Cube", ImVec2(50, 20)))
		ps.init("init_particles_cube");
	if (ImGui::Button("Rect", ImVec2(50, 20)))
		ps.init("init_particles_rect");
	if (ImGui::Button("Sine", ImVec2(50, 20)))
		ps.init("init_particles_sine");

	ImGui::Separator();
	ImGui::Text("Gravity points: %lu", ps.gravityPoints.size());
	ImGui::Checkbox("Show", &ps.renderGravityPoints);
	if (ImGui::Button("Clear", ImVec2(50, 20)))
		ps.gravityPoints.clear();

	for (size_t i = 0; i < ps.gravityPoints.size(); i++)
	{
		cl_float4 p = ps.gravityPoints[i];
		float v[3] = {p.s[0], p.s[1], p.s[2]};
		if (ImGui::DragFloat3(std::to_string(i).c_str(), v, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps.gravityPoints[i].s[0] = v[0];
			ps.gravityPoints[i].s[1] = v[1];
			ps.gravityPoints[i].s[2] = v[2];
		}
	}
	
	ImGui::End();

	ImGui::Begin("opencl");
	ImGui::Text("Platform: %s, %s", ps.clCtx.clInfo.platformVendor, ps.clCtx.clInfo.platformName);
	ImGui::Text("Device: %s", ps.clCtx.clInfo.deviceName);
	ImGui::Separator();
	ImGui::Text("Program");
	for (auto k : ps.clProgram->clKernels)
	{
		ImGui::Text("%s", k->name);
	}
	ImGui::End();

	ImGui::Begin("opengl");
	ImGui::Text("GL version: %d.%d", ps.glCtx.glInfo.glMajorVersion, ps.glCtx.glInfo.glMinorVersion);
	ImGui::Text("GLSL version: %s", ps.glCtx.glInfo.shadingLanguageVersion);
	ImGui::Text("Vendor: %s", ps.glCtx.glInfo.vendor);
	ImGui::Text("Renderer: %s", ps.glCtx.glInfo.renderer);
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

GUIContext::~GUIContext(){}
