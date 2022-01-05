#include "GUIContext.h"

GUIContext::GUIContext()
{
}

void GUIContext::Init(GLFWwindow *window, const char *glslVersion)
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}
void GUIContext::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hi");
	ImGui::Text("This is text");
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

GUIContext::~GUIContext()
{
}
