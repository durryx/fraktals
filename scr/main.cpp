#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>
#include <glad/glad.h>
#include <string>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
/*
#include "../libs/glad/include/glad/glad.h"
#include "../libs/glfw/include/GLFW/glfw3.h"
*/
#include "globals.hpp"
#include "callback_functions.hpp"
#include "koch.hpp"
#include "../libs/imgui/imgui.h"
#include "../libs/imgui/imgui_impl_glfw.h"
#include "../libs/imgui/imgui_impl_opengl3.h"

struct fractal_name
{
    int id;
    std::string name;
};

int main(int argc, char** argv)
{

    fractal_name fractals[] = 
    {
        {0, "Koch Snowflake"},
        {1, "Koch Surface"},
        {2, "3D Koch Snowflake"}
    };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* main_window = glfwCreateWindow(700, 500, "fraktals", NULL, NULL);
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
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize.x = 700.0f;
    io.DisplaySize.y = 500.0f;

    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // to do a queue of objects
    main_window_wrapper.fractal = 0;
    koch koch1(main_window_wrapper);
    koch1.koch_shaders.current_shader_ID = koch1.koch_shaders.link_shader_program(
        koch1.koch_shaders.vertex_shader_ID_list[0],
        koch1.koch_shaders.fragment_shader_ID_list[0]);

    int frames = 0;
    double t, t0, fps;
    t0 = glfwGetTime();

    while(!glfwWindowShouldClose(main_window))
    {
 
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



        if(ImGui::BeginMainMenuBar())
        {
            std::string s = std::string("Fractal: ") + fractals[main_window_wrapper.fractal].name;
            if(ImGui::BeginMenu(s.c_str()))
            {
                for(fractal_name x : fractals)
                {
                    if (ImGui::MenuItem(x.name.c_str())) {main_window_wrapper.fractal = x.id;};
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::ShowDemoWindow();



        koch1.draw_koch_snowflake(VAO, VBO); 
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwGetFramebufferSize(main_window, &main_window_wrapper.window_width, &main_window_wrapper.window_height);
        
        t = glfwGetTime();
        if((t - t0) > 1.0 || frames == 0)
        {
            fps = (double)frames / (t-t0);
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    return 0;
}
