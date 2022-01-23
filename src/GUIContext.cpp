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
	ImGui::Text("FPS %d", ps.fps);
	ImGui::Text("Particles %zu", ps.numParticles);
	ImGui::SliderFloat("Size", &ps.particleSize, 1.0f, 5.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	ImGui::Separator();
	ImGui::Text("Camera");
	ImGui::Text("Position x %f, y %f, z %f", ps.camera.position.x, ps.camera.position.y, ps.camera.position.z);
	ImGui::Text("Yaw %f, Pitch %f", ps.camera.yaw, ps.camera.pitch);
	ImGui::Checkbox("Movement", &ps.mouseMovement);
	ImGui::ColorEdit4("Clear color", &ps.gl.clearColor.r);
	ImGui::Separator();
	ImGui::Text("Mouse");
	ImGui::Text("Screen x %f, y %f, z %f", ps.mouseInfo.screen.x, ps.mouseInfo.screen.y, ps.mouseInfo.screen.z);
	ImGui::Text("NDC x %f, y %f, z %f", ps.mouseInfo.ndc.x, ps.mouseInfo.ndc.y, ps.mouseInfo.ndc.z);
	ImGui::Text("World x %f, y %f, z %f", ps.mouseInfo.world.x, ps.mouseInfo.world.y, ps.mouseInfo.world.z);
	ImGui::Checkbox("Gravity", (bool*)&ps.mouseGravity);
	ImGui::SameLine();
	ImGui::SliderFloat("Mass", &ps.mouseGravityScale, 1.0, 100.0, "%.2f", ImGuiSliderFlags_None);
	ImGui::Separator();

	ImGui::ColorEdit4("Min color", &ps.minColor.x);
	ImGui::ColorEdit4("Max color", &ps.maxColor.x);
	ImGui::Separator();
	ImGui::Text("Reset");
	if (ImGui::Button("Sphere", ImVec2(50, 20)))
		ps.InitParticles(SPHERE);
	ImGui::SameLine();
	if (ImGui::Button("Cube", ImVec2(50, 20)))
		ps.InitParticles(CUBE);
	ImGui::SameLine();
	if (ImGui::Button("Rect", ImVec2(50, 20)))
		ps.InitParticles(RECT);
	ImGui::SameLine();
	if (ImGui::Button("Sine", ImVec2(50, 20)))
		ps.InitParticles(SINE);
	ImGui::SameLine();
	if (ImGui::Button("Emitter", ImVec2(50, 20)))
		ps.InitParticlesEmitter();


	ImGui::Separator();
	ImGui::Text("Emitter");
	ImGui::Checkbox("Active", &ps.useEmitter);
	if (ps.useEmitter)
	{
		ImGui::DragFloat("Rate", &ps.emitter.rate);
		ImGui::DragFloat("Life", &ps.emitter.life);
		ImGui::DragFloat("Speed", &ps.emitter.velocity);
		float v[3] = {ps.emitter.position.x, ps.emitter.position.y, ps.emitter.position.z};
		if (ImGui::DragFloat3("Position", v, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps.emitter.position = glm::vec3(v[0], v[1], v[2]);
		}
		float v2[3] = {ps.emitter.direction.x, ps.emitter.direction.y, ps.emitter.direction.z};
		if (ImGui::DragFloat3("Direction", v2, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps.emitter.direction = glm::vec3(v2[0], v2[1], v2[2]);
		}
	}
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
	ImGui::Text("Platform: %s, %s", ps.cl.clInfo.platformVendor, ps.cl.clInfo.platformName);
	ImGui::Text("Device: %s", ps.cl.clInfo.deviceName);
	ImGui::Separator();
	ImGui::Text("Program");
	for (auto k : ps.clProgram->clKernels)
	{
		ImGui::Text("%s", k->name);
	}
	ImGui::End();

	ImGui::Begin("opengl");
	ImGui::Text("GL version: %d.%d", ps.gl.glInfo.glMajorVersion, ps.gl.glInfo.glMinorVersion);
	ImGui::Text("GLSL version: %s", ps.gl.glInfo.shadingLanguageVersion);
	ImGui::Text("Vendor: %s", ps.gl.glInfo.vendor);
	ImGui::Text("Renderer: %s", ps.gl.glInfo.renderer);
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
