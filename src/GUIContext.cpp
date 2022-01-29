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
	ImGui::Checkbox("Show Overlays", &ps.showOverlays);
	ImGui::Separator();

	ImGui::Text("Camera");
	ImGui::Text("Position x %f, y %f, z %f", ps.camera.position.x, ps.camera.position.y, ps.camera.position.z);
	ImGui::Text("Yaw %f, Pitch %f", ps.camera.yaw, ps.camera.pitch);
	if (ImGui::SmallButton("Front"))
	{
		ps.camera.Reset(glm::vec3(0.0, 0.0, 2.0), -90.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Back"))
	{
		ps.camera.Reset(glm::vec3(0.0, 0.0, -2.0), 90.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Left"))
	{
		ps.camera.Reset(glm::vec3(-2.0, 0.0, 0.0), 0.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Right"))
	{
		ps.camera.Reset(glm::vec3(2.0, 0.0, 0.0), 180.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Top"))
	{
		ps.camera.Reset(glm::vec3(0.0, 2.0, 0.0), -90.0f, -89.0f);
	}
	ImGui::Separator();

	ImGui::Text("Mouse");
	ImGui::Text("Screen x %f, y %f, z %f", ps.mouseInfo.screen.x, ps.mouseInfo.screen.y, ps.mouseInfo.screen.z);
	ImGui::Text("NDC x %f, y %f, z %f", ps.mouseInfo.ndc.x, ps.mouseInfo.ndc.y, ps.mouseInfo.ndc.z);
	ImGui::Text("World x %f, y %f, z %f", ps.mouseInfo.world.x, ps.mouseInfo.world.y, ps.mouseInfo.world.z);
	ImGui::Checkbox("Gravity", (bool*)&ps.mouseGravity);
	ImGui::SameLine();
	ImGui::SliderFloat("Mass", &ps.mouseGravityScale, 1.0, 100.0, "%.2f", ImGuiSliderFlags_None);
	ImGui::Separator();

	ImGui::Text("Particle Settings");
	ImGui::Text("Count %zu", ps.numParticles);
	ImGui::SliderFloat("Size", &ps.particleSize, 1.0f, 5.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	ImGui::ColorEdit3("Background color", &ps.gl.clearColor.r);
	ImGui::Text("Reset");
	if (ImGui::SmallButton("Sphere"))
		ps.InitParticles(SPHERE);
	ImGui::SameLine();
	if (ImGui::SmallButton("Torus"))
		ps.InitParticles(TORUS);
	ImGui::SameLine();
	if (ImGui::SmallButton("Cube"))
		ps.InitParticles(CUBE);
	ImGui::SameLine();
	if (ImGui::SmallButton("Rect"))
		ps.InitParticles(RECT);
	ImGui::SameLine();
	if (ImGui::SmallButton("Sine"))
		ps.InitParticles(SINE);
	ImGui::SameLine();
	if (ImGui::SmallButton("Emitter"))
	{
		ps.InitParticlesEmitter();
		ps.useEmitter = true;
	}

	ImGui::Separator();
	if (ps.useEmitter)
	{
		ImGui::Text("Emitter");
		ImGui::ColorEdit4("Min color", &ps.minColor.x);
		ImGui::ColorEdit4("Max color", &ps.maxColor.x);
		if (ImGui::SliderFloat("Radius", &ps.emitter.radius, 0.01f, 1.0f, "%.2f", ImGuiSliderFlags_None))
			ps.InitParticlesEmitter();
		if (ImGui::SliderFloat("Rate", &ps.emitter.rate, 1, 50000))
			ps.InitParticlesEmitter();
		if (ImGui::SliderFloat("Life", &ps.emitter.life, 0.1, 20.0, "%.2f", ImGuiSliderFlags_None))
			ps.InitParticlesEmitter();
		if (ImGui::SliderFloat("Speed", &ps.emitter.velocity, 1000, 20000, "%f", ImGuiSliderFlags_None))
			ps.InitParticlesEmitter();
		float v[3] = {ps.emitter.position.x, ps.emitter.position.y, ps.emitter.position.z};
		if (ImGui::DragFloat3("Position", v, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps.emitter.position = glm::vec3(v[0], v[1], v[2]);
			ps.InitParticlesEmitter();
		}
		float v2[3] = {ps.emitter.direction.x, ps.emitter.direction.y, ps.emitter.direction.z};
		if (ImGui::DragFloat3("Rotation", v2, 0.1, -180.0, 180.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps.emitter.direction = glm::vec3(v2[0], v2[1], v2[2]);
			ps.InitParticlesEmitter();
		}
		ImGui::Separator();
	}
	ImGui::Text("Gravity points: %lu", ps.gravityPoints.size());
	if (ImGui::Button("Clear", ImVec2(50, 20)))
		ps.gravityPoints.clear();

	for (size_t i = 0; i < ps.gravityPoints.size(); i++)
	{
		cl_float4 p = ps.gravityPoints[i];
		float v[4] = {p.s[0], p.s[1], p.s[2], p.s[3]};
		if (ImGui::DragFloat4(std::to_string(i).c_str(), v, 0.01, -100.0, 100.0, "%.3f", ImGuiSliderFlags_None))
		{
			ps.gravityPoints[i].s[0] = v[0];
			ps.gravityPoints[i].s[1] = v[1];
			ps.gravityPoints[i].s[2] = v[2];
			ps.gravityPoints[i].s[3] = v[3];
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
