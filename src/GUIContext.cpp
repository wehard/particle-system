#include "GUIContext.h"

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
	ImGui::Text("Particles %zu", ps.numParticles);
	ImGui::Text("Camera x %f, y %f, z %f", ps.glCtx.camera->position.x, ps.glCtx.camera->position.y, ps.glCtx.camera->position.z);
	ImGui::Text("Mouse x %f, y %f, z %f", ps.m_pos.x, ps.m_pos.y, ps.m_pos.z);
	if (ImGui::Button("Reset", ImVec2(50, 20)))
		ps.reset();
	ImGui::End();

	ImGui::Begin("opencl");
	ImGui::Text("Platform: %s, %s", ps.clCtx.clInfo.platformVendor, ps.clCtx.clInfo.platformName);
	ImGui::Text("Device: %s", ps.clCtx.clInfo.deviceName);
	ImGui::Separator();
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
