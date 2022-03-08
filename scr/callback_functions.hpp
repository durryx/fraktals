#ifndef CALLBACK_FUNCTIONS_HPP
#define CALLBACK_FUNCTIONS_HPP

#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
/*
#include "../libs/glfw/include/GLFW/glfw3.h"
#include "../libs/glm/glm/glm.hpp"
*/
#include "globals.hpp"

class variable_wrapper
{
    public:
        int fractal;

        int window_width;
        int window_height;
        double mouse_xpos;
        double mouse_ypos;
        double old_mouse_x_offset;
        double mouse_y_offset;
        double old_mouse_y_offset;
        double mouse_x_offset;

        glm::mat4 model_mat;
        glm::mat4 projection_mat;
        glm::mat4 view_mat;
        float fov;

        bool koch_next_iteration;
        int koch_mode;
        int koch_last_mode;
        bool koch_switch_mode;
        bool koch_status_calculating;
        bool koch_grid;

    variable_wrapper();

    ~variable_wrapper();
};


void callback_resize_func(GLFWwindow* main_window, int width, int height);

void key_callback(GLFWwindow* main_window, int key, int scancode, int action, int mods);

//void move_on_line(float delta);

void mouse_callback(GLFWwindow *main_window, double xpos, double ypos);

void scroll_callback(GLFWwindow *main_window, double x, double y);

#endif