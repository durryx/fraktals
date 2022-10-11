#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
/*
#include "../libs/glad/include/glad/glad.h"
#include "../libs/glfw/include/GLFW/glfw3.h"
*/
//#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include "callback_functions.hpp"
#include "globals.hpp"
#include "koch.hpp"
//#include <imgui.h>

struct fractal_name
{
    int id;
    std::string name;
};

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char* message, const void* userParam);

int main(int argc, char** argv)
{
    fractal_name fractals[] = {
        {0, "Koch Snowflake"}, {1, "Koch Surface"}, {2, "3D Koch Snowflake"}};

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // debug option

    GLFWwindow* main_window =
        glfwCreateWindow(700, 500, "fraktals", NULL, NULL);
    if (main_window == NULL)
    {
        glfwTerminate();
        // exception
    }
    glfwSwapInterval(1);

    variable_wrapper main_window_wrapper;
    glfwSetWindowUserPointer(main_window, &main_window_wrapper);

    glfwMakeContextCurrent(main_window);
    glfwSetFramebufferSizeCallback(main_window, callback_resize_func);
    glfwSetKeyCallback(main_window, key_callback);
    glfwSetCursorPosCallback(main_window, mouse_callback);
    glfwSetScrollCallback(main_window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        // exception
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                              nullptr, GL_TRUE);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.DisplaySize.x = 700.0f;
    io.DisplaySize.y = 500.0f;

    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
    const char* glsl_version = "#version 460";
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // to do a queue of objects
    main_window_wrapper.fractal = 0;

    koch koch1(main_window_wrapper);
    koch1.koch_shaders.current_shader_ID =
        koch1.koch_shaders.link_shader_program(
            koch1.koch_shaders.vertex_shader_ID_list[0],
            koch1.koch_shaders.fragment_shader_ID_list[0]);

    int frames = 0;
    double t, t0, fps;
    t0 = glfwGetTime();

    while (!glfwWindowShouldClose(main_window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar())
        {
            std::string s = std::string("Fractal: ") +
                            fractals[main_window_wrapper.fractal].name;
            if (ImGui::BeginMenu(s.c_str()))
            {
                for (fractal_name x : fractals)
                {
                    if (ImGui::MenuItem(x.name.c_str()))
                    {
                        main_window_wrapper.fractal = x.id;
                    };
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::ShowDemoWindow();

        koch1.draw_koch_snowflake();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwGetFramebufferSize(main_window, &main_window_wrapper.window_width,
                               &main_window_wrapper.window_height);

        t = glfwGetTime();
        if ((t - t0) > 1.0 || frames == 0)
        {
            fps = (double)frames / (t - t0);
            t0 = t;
            frames = 0;
        }
        frames++;

        glfwSwapBuffers(main_window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}