#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
/*
#include "../libs/glfw/include/GLFW/glfw3.h"
#include "../libs/glm/glm/glm.hpp"
*/
#include "globals.hpp"
#include "callback_functions.hpp"
#include "koch.hpp"

variable_wrapper::variable_wrapper()
    : projection_mat(glm::mat4(1.0f)), model_mat(glm::mat4(1.0f)),
      view_mat(glm::mat4(1.0f)), window_height(500), window_width(700), fractal(0)
{

}

variable_wrapper::~variable_wrapper()
{

}

void callback_resize_func(GLFWwindow* main_window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//void move_on_line(float delta)
//{
//    glm::vec3 mouse((float)mouse_xpos, (float)window_height - (float)mouse_ypos, 100.0f);
//
//    // fails when matrix is non-invertible, determinant is zero
//    glm::vec3 world_proj = glm::unProject(mouse, view_mat, projection_mat, glm::vec4(0.0f, 0.0f, (float)window_width, (float)window_height));
//
//    glm::vec3 eye = glm::vec3(glm::inverse(view_mat)[3]);
//
//    glm::vec3 ray = glm::normalize(world_proj - eye);
//
//    glm::mat4 inv_view = glm::translate(glm::mat4(1.0f), ray*delta) * glm::inverse(view_mat);
//
//    view_mat = glm::inverse(inv_view);
//
//}

void mouse_callback(GLFWwindow *main_window, double xpos, double ypos)
{
    void *data = glfwGetWindowUserPointer(main_window);
    variable_wrapper *wrapper = reinterpret_cast<variable_wrapper*>(data);

    wrapper->mouse_xpos = xpos;
    wrapper->mouse_ypos = ypos;
}

void scroll_callback(GLFWwindow *main_window, double x, double y)
{
    void *data = glfwGetWindowUserPointer(main_window);
    variable_wrapper *wrapper = reinterpret_cast<variable_wrapper*>(data);

    wrapper->old_mouse_x_offset = wrapper->mouse_x_offset; 
    wrapper->old_mouse_y_offset = wrapper->mouse_y_offset; 
    wrapper->mouse_y_offset -= y;
    wrapper->mouse_x_offset -= x;


    switch (wrapper->fractal)
    {
    case 0:
        koch_mouse_scrollback(wrapper);
        break;
    
    default:
        break;
    }

    //move_on_line(yoffset*0.001f);
}

void key_callback(GLFWwindow* main_window, int key, int scancode, int action, int mods)
{
    void *data = glfwGetWindowUserPointer(main_window);
    variable_wrapper *wrapper = reinterpret_cast<variable_wrapper*>(data);

    switch (key)
    {
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(main_window, true);
        break;
    default:
        break;
    }
    switch (wrapper->fractal)
    {
    case 0:
        koch_keyboard_input(wrapper, key, action, mods);
        break;
    default:
        break;
    }
}
