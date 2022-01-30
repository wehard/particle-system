#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ParticleSystem.h"
#include "Application.h"
#include "CLContext.h"

class GUIContext
{
private:
    ImGuiContext *context;
    void UpdatePlatformInfo(Application &app);
    void UpdateSystems(Application &app);
    void UpdateApplicationInfo(Application &app);
public:
    GUIContext();
    void Init(GLFWwindow *window, const char *glslVersion);
    void Update(ParticleSystem &ps);
    void Update(Application &app);
    void Render();
    void Shutdown();
    ~GUIContext();
};

