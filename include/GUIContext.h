#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GUIContext
{
private:
    ImGuiContext *context;
public:
    GUIContext();
    void Init(GLFWwindow *window, const char *glslVersion);
    void Update();
    void Render();
    void Shutdown();
    ~GUIContext();
};

