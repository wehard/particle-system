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
	ImGui::Text("FPS %d", ps.glCtx.fps);
	ImGui::Text("Particles %zu", ps.numParticles);
	ImGui::Separator();
	ImGui::Text("Camera x %f, y %f, z %f", ps.glCtx.camera->position.x, ps.glCtx.camera->position.y, ps.glCtx.camera->position.z);
	ImGui::Separator();
	ImGui::Text("Mouse");
	ImGui::Text("Screen x %f, y %f, z %f", ps.mouseInfo.screen.x, ps.mouseInfo.screen.y, ps.mouseInfo.screen.z);
	ImGui::Text("NDC x %f, y %f, z %f", ps.mouseInfo.ndc.x, ps.mouseInfo.ndc.y, ps.mouseInfo.ndc.z);
	ImGui::Text("World x %f, y %f, z %f", ps.mouseInfo.world.x, ps.mouseInfo.world.y, ps.mouseInfo.world.z);
	ImGui::Text("World2 x %f, y %f, z %f", ps.mouseInfo.world2.x, ps.mouseInfo.world2.y, ps.mouseInfo.world2.z);
	ImGui::Separator();

	float v[3] = {ps.rotation.x, ps.rotation.y, ps.rotation.z};
	if (ImGui::DragFloat3("Rotation", v, 0.10, 0.0, 360.0, "%.1f", ImGuiSliderFlags_None))
	{
		ps.rotation = glm::vec3(v[0], v[1], v[2]);
	}

	ImGui::Text("Reset");
	if (ImGui::Button("Sphere", ImVec2(50, 20)))
		ps.init("init_particles_sphere");
	if (ImGui::Button("Cube", ImVec2(50, 20)))
		ps.init("init_particles_cube");
	if (ImGui::Button("Rect", ImVec2(50, 20)))
		ps.init("init_particles_rect");
	if (ImGui::Button("Sine", ImVec2(50, 20)))
		ps.init("init_particles_sine");
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
