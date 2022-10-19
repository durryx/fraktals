#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <functional>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/scalar_multiplication.hpp>
#include <iostream>
#include <mutex>
#include <numbers>
#include <queue>
#include <thread>
#include <vector>
/*
#include "../libs/glfw/include/GLFW/glfw3.h"
#include "../libs/glm/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/glm/gtc/type_ptr.hpp"
*/
//#include <imgui.h>
//#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"

#include "globals.hpp"
#include "koch.hpp"
#include "shader.hpp"

using namespace std::chrono;

void koch::draw_koch_snowflake()
{

    // put image and info here

    ImGui::Begin("Koch Snowflake settings");
    ImGui::Text("Current mode");
    if (ImGui::RadioButton("single thread mode", &wrapper.koch_mode, 0))
    {
        wrapper.koch_switch_mode = true;
    };
    if (ImGui::RadioButton("multithread mode", &wrapper.koch_mode, 1))
    {
        wrapper.koch_switch_mode = true;
    };

    if (wrapper.koch_switch_mode)
    {
        wrapper.koch_next_iteration = false;
        wrapper.koch_switch_mode = false;
        switch (wrapper.koch_mode)
        {
            case 0:
                load_singlethread();
                clean_multithreads();
                break;
            case 1:
                if ((float)vertexlist_p_s.size() / (processor_count * 4) <= 1)
                {
                    wrapper.koch_mode = 0;
                    break;
                }
                load_points_multithread(vertexlist_p_s);
                clean_singlethread();
                init_threads();
                gen_color_palette();
                break;
            case 2:
                // nothing...
                break;
            default:
                // throw error;
                break;
        }
    }

    unsigned int transformationLoc;
    glm::mat4 transformation =
        wrapper.projection_mat * wrapper.view_mat * wrapper.model_mat;

    switch (wrapper.koch_mode)
    {
        case 0:
            koch_shaders.use(koch_shaders.shader_program_IDs[0]);
            transformationLoc = glGetUniformLocation(
                koch_shaders.current_shader_ID, "transformation");
            wrapper.projection_mat =
                glm::ortho(-(float)wrapper.window_width / 2,
                           (float)wrapper.window_width / 2,
                           -(float)wrapper.window_height / 2,
                           (float)wrapper.window_height / 2, 0.0f, 100.0f);
            // wrapper.projection_mat =
            // glm::perspective(glm::radians(wrapper.fov),
            // (float)wrapper.window_width/(float)wrapper.window_height, 0.0f,
            // 100.0f);

            glUniformMatrix4fv(transformationLoc, 1, GL_FALSE,
                               glm::value_ptr(transformation));
            koch::handle_single_thread();
            break;
        case 1:
            koch_shaders.use(koch_shaders.shader_program_IDs[0]);
            transformationLoc = glGetUniformLocation(
                koch_shaders.current_shader_ID, "transformation");

            wrapper.projection_mat =
                glm::ortho(-(float)wrapper.window_width / 2,
                           (float)wrapper.window_width / 2,
                           -(float)wrapper.window_height / 2,
                           (float)wrapper.window_height / 2, 0.0f, 100.0f);
            // wrapper.projection_mat =
            // glm::perspective(glm::radians(wrapper.fov),
            // (float)wrapper.window_width/(float)wrapper.window_height, 0.0f,
            // 100.0f);

            glUniformMatrix4fv(transformationLoc, 1, GL_FALSE,
                               glm::value_ptr(transformation));
            koch::handle_multithread();
            break;
        default:
            // throw error;
            break;
    }
    ImGui::End();
}

void koch::refuse_switch(std::string current_mode, std::string last_mode)
{
    if (ImGui::BeginPopup("Error"))
    {
        std::string txt = std::string("Cannot switch from ") + last_mode +
                          std::string(" mode to ") + current_mode +
                          std::string(" mode");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), txt.c_str());
        ImGui::EndPopup();
    }
}

void koch::gen_objects()
{
    glGenVertexArrays(1, &VAO_p);
    glGenBuffers(1, &VBO_p);

    glGenVertexArrays(1, &VAO_m);
    glGenBuffers(processor_count, VBO_m.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Singlethread mode */

bool koch::insert_new_points()
{
    double dx, dy;
    if (point_counter == (vertexlist_p_s.size() - 2))
    {
        dx = vertexlist_p_s[0] - vertexlist_p_s[point_counter];
        dy = vertexlist_p_s[1] - vertexlist_p_s[point_counter + 1];
    }

    else
    {
        dx = vertexlist_p_s[point_counter + 2] - vertexlist_p_s[point_counter];
        dy = vertexlist_p_s[point_counter + 3] -
             vertexlist_p_s[point_counter + 1];
    }

    std::array<double, 8> temp = {
        vertexlist_p_s[point_counter],
        vertexlist_p_s[point_counter + 1],

        vertexlist_p_s[point_counter] + dx / 3,
        vertexlist_p_s[point_counter + 1] + dy / 3,

        vertexlist_p_s[point_counter] + dx / 2 - dy * sin60 / 3,
        vertexlist_p_s[point_counter + 1] + dy / 2 + dx * sin60 / 3,

        vertexlist_p_s[point_counter] + 2 * dx / 3,
        vertexlist_p_s[point_counter + 1] + 2 * dy / 3};

    glBufferSubData(GL_ARRAY_BUFFER, vertexlist_s.size() * sizeof(double),
                    sizeof(temp), temp.data());

    vertexlist_s.insert(vertexlist_s.end(), temp.begin(), temp.end());

    if (point_counter == vertexlist_p_s.size() - 2)
        return true;
    else
    {
        point_counter += 2;
        return false;
    }
}

void koch::handle_single_thread()
{
    glBindVertexArray(VAO_p);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_p);

    if (wrapper.koch_next_iteration)
    {
        if (finished)
        {
            glBufferData(GL_ARRAY_BUFFER,
                         4 * vertexlist_p_s.size() * sizeof(double), NULL,
                         GL_DYNAMIC_DRAW);
            finished = insert_new_points();
        }
        else
        {
            finished = insert_new_points();
            if (finished)
            {
                point_counter = 0;
                vertexlist_p_s = vertexlist_s;
                vertexlist_s.clear();
                iterations++;
                vertexlist_s.reserve(6 * pow(4, iterations));
                wrapper.koch_next_iteration = false;
            }
        }
    }

    glVertexAttribLPointer(0, 2, GL_DOUBLE, 2 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);

    if (finished)
        glDrawArrays(GL_LINE_LOOP, 0, vertexlist_p_s.size() / 2);
    else
        glDrawArrays(GL_LINE_STRIP, 0, vertexlist_s.size() / 2);
}

void koch::clean_singlethread()
{
    vertexlist_p_s.clear();
    vertexlist_s.clear();
}

/* Multithreaded mode */

void koch::allocate_number_of_points_memory_per_thread(
    std::vector<std::vector<double>>& points, int num)
{
    for (int x = 0; x < processor_count; x++)
    {
        points[x].clear();
        points[x].resize(num);
    }
}

void koch::load_points_multithread(std::vector<double>& dat)
{
    int points_per_thread = dat.size() / processor_count;
    size_t j = 0;
    GLbitfield flags =
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    for (size_t x = 0; x < vertexlist_m.size(); x++)
    {
        if (x == 0)
        {
            std::vector<double> temp = {dat.begin(),
                                        dat.begin() + (j += points_per_thread)};
            vertexlist_p_m[x].insert(vertexlist_p_m[x].end(), temp.begin(),
                                     temp.end());
            glBindBuffer(GL_ARRAY_BUFFER, VBO_m[x]);
            glBufferStorage(GL_ARRAY_BUFFER, points_per_thread * sizeof(double),
                            NULL, flags);
            double* map = reinterpret_cast<double*>(glMapBufferRange(
                GL_ARRAY_BUFFER, 0, points_per_thread * sizeof(double), flags));
            std::copy(temp.begin(), temp.end(), map);
            VBO_address.push_back(map);
        }
        else if (x == processor_count - 1)
        {
            std::vector<double> temp = {dat.begin() + j, dat.end()};
            vertexlist_p_m[x].insert(vertexlist_p_m[x].end(), temp.begin(),
                                     temp.end());
            glBindBuffer(GL_ARRAY_BUFFER, VBO_m[x]);
            glBufferStorage(GL_ARRAY_BUFFER, points_per_thread * sizeof(double),
                            NULL, flags);
            double* map = reinterpret_cast<double*>(glMapBufferRange(
                GL_ARRAY_BUFFER, 0, points_per_thread * sizeof(double), flags));
            std::copy(temp.begin(), temp.end(), map);
            VBO_address.push_back(map);
        }
        else
        {
            std::vector<double> temp = {dat.begin() + j,
                                        dat.begin() + (j += points_per_thread)};
            vertexlist_p_m[x].insert(vertexlist_p_m[x].end(), temp.begin(),
                                     temp.end());
            glBindBuffer(GL_ARRAY_BUFFER, VBO_m[x]);
            glBufferStorage(GL_ARRAY_BUFFER, points_per_thread * sizeof(double),
                            NULL, flags);
            double* map = reinterpret_cast<double*>(glMapBufferRange(
                GL_ARRAY_BUFFER, 0, points_per_thread * sizeof(double), flags));
            std::copy(temp.begin(), temp.end(), map);
            VBO_address.push_back(map);
        }
        VBO_vertex_counter[x] = points_per_thread;
    }
}

void koch::gen_color_palette()
{
    // implement palette generation based on number of cpu cores
    colors = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},

              {0.5f, 0.5f, 0.0f}, {0.5f, 0.0f, 0.5f}, {0.0f, 0.5f, 0.5f}};
}

void koch::init_threads()
{
    running = true;
    for (int i = 0; i < processor_count; i++)
    {
        pool.push_back(std::thread(&koch::infinite_loop_function, this, i));
    }
}

void koch::infinite_loop_function(int thread_number)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        data_cond[thread_number].wait(
            lock,
            [this] { return !assigned_vertexlist_index.empty() || !running; });
        if (!running)
            break;
        int vertexlist_index = assigned_vertexlist_index.front();
        assigned_vertexlist_index.pop();
        lock.unlock();

        thread_status_mutex.lock();
        thread_status[thread_number] = false;
        thread_status_mutex.unlock();

        int i = 0;
        while (i <= vertexlist_p_m[vertexlist_index].size() - 2)
        {
            double dx, dy;
            if (i == vertexlist_p_m[vertexlist_index].size() - 2)
            {
                if (vertexlist_index == vertexlist_p_m.size() - 1)
                {
                    dx = vertexlist_p_m[0][0] -
                         vertexlist_p_m[vertexlist_index][i];
                    dy = vertexlist_p_m[0][1] -
                         vertexlist_p_m[vertexlist_index][i + 1];
                }
                else
                {
                    dx = vertexlist_p_m[vertexlist_index + 1][0] -
                         vertexlist_p_m[vertexlist_index][i];
                    dy = vertexlist_p_m[vertexlist_index + 1][1] -
                         vertexlist_p_m[vertexlist_index][i + 1];
                }
            }
            else
            {
                dx = vertexlist_p_m[vertexlist_index][i + 2] -
                     vertexlist_p_m[vertexlist_index][i];
                dy = vertexlist_p_m[vertexlist_index][i + 3] -
                     vertexlist_p_m[vertexlist_index][i + 1];
            }

            std::array<double, 8> temp = {
                vertexlist_p_m[vertexlist_index][i],
                vertexlist_p_m[vertexlist_index][i + 1],

                vertexlist_p_m[vertexlist_index][i] + dx / 3,
                vertexlist_p_m[vertexlist_index][i + 1] + dy / 3,

                vertexlist_p_m[vertexlist_index][i] + dx / 2 - dy * sin60 / 3,
                vertexlist_p_m[vertexlist_index][i + 1] + dy / 2 +
                    dx * sin60 / 3,

                vertexlist_p_m[vertexlist_index][i] + 2 * dx / 3,
                vertexlist_p_m[vertexlist_index][i + 1] + 2 * dy / 3};
            vertexlist_m[vertexlist_index].insert(
                vertexlist_m[vertexlist_index].end(), temp.begin(), temp.end());

            int j = 0;
            while (stop_VBO)
                ;
            access_opengl[thread_number] = true;
            for (double x : temp)
            {
                VBO_address[thread_number][(i * 4 + j)] = x;
                j++;
            }
            VBO_vertex_counter[thread_number] += 8;
            access_opengl[thread_number] = false;
            i += 2;
            std::this_thread::sleep_for(4ns);
        }
        thread_status_mutex.lock();
        thread_status[thread_number] = true;
        thread_status_mutex.unlock();
    }
}

bool koch::check_finished()
{
    std::lock_guard<std::mutex> guard(thread_status_mutex);
    if (std::all_of(thread_status.begin(), thread_status.end(),
                    [](bool v) { return v; }))
        return true;
    else
        return false;
}

void koch::refill_queue()
{
    for (int i = 0; i < processor_count; i++)
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        assigned_vertexlist_index.push(i);
        data_cond[i].notify_one();
    }
}

void koch::clear_vec(std::vector<std::vector<double>>& points)
{
    for (int i = 0; i < processor_count; i++)
        points[i].clear();
}

void koch::lock_threads()
{
    stop_VBO = true;
    while (true)
    {
        if (std::all_of(access_opengl.begin(), access_opengl.end(),
                        [](bool i)
                        { return !i; })) // access_opengl is true when accessed
            break;
    }
}

void koch::lock_VBOs()
{
    if (sync_VBOs_object)
        glDeleteSync(sync_VBOs_object);

    sync_VBOs_object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void koch::wait_VBOs()
{
    if (sync_VBOs_object)
    {
        while (1)
        {
            GLenum waitReturn = glClientWaitSync(sync_VBOs_object,
                                                 GL_SYNC_FLUSH_COMMANDS_BIT, 0);
            if (waitReturn == GL_ALREADY_SIGNALED ||
                waitReturn == GL_CONDITION_SATISFIED)
                return;
        }
    }
}

void koch::invalidate_all_VBOs()
{
    glDeleteBuffers(processor_count, VBO_m.data());
    VBO_m.clear();
    VBO_m.resize(processor_count);
    VBO_address.clear();
    glGenBuffers(processor_count, VBO_m.data());
    GLbitfield flags =
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    for (unsigned int VBO_x : VBO_m)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_x);

        glBufferStorage(GL_ARRAY_BUFFER,
                        4 * VBO_vertex_counter[0] * sizeof(double), NULL,
                        flags);
        double* map = reinterpret_cast<double*>(glMapBufferRange(
            GL_ARRAY_BUFFER, 0, 4 * VBO_vertex_counter[0] * sizeof(double),
            flags));
        VBO_address.push_back(map);
    }

    for (auto& i : VBO_vertex_counter)
        i = 0;
}

void koch::reserve_mem()
{
    std::for_each(vertexlist_m.begin(), vertexlist_m.end(),
                  [](std::vector<double>& row)
                  {
                      row.clear();
                      row.resize(row.size() * 4);
                  });
}

void koch::draw_multithread()
{
    for (size_t i = 0; unsigned int VBO_x : VBO_m)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_x);
        glVertexAttribLPointer(0, 2, GL_DOUBLE, 2 * sizeof(double), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINE_STRIP, 0, VBO_vertex_counter[i].load() / 2);
        i++;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void koch::handle_multithread()
{
    if (wrapper.koch_next_iteration)
    {
        if (finished)
        {
            invalidate_all_VBOs();
            refill_queue();
            lock_threads();

            draw_multithread();

            stop_VBO = false;
            finished = false;
        }
        else if (check_finished())
        {
            finished = true;
            iterations++;
            wrapper.koch_next_iteration = false;
            vertexlist_p_m = vertexlist_m;
            reserve_mem();
            draw_multithread();
        }
        else
        {
            lock_threads();
            draw_multithread();
            stop_VBO = false;
        }
    }
    else
        draw_multithread();
}

void koch::load_singlethread()
{
    for (size_t i = 0; i < processor_count; i++)
        vertexlist_p_s.insert(vertexlist_p_s.end(), vertexlist_p_m[i].begin(),
                              vertexlist_p_m[i].end());
}

void koch::clean_multithreads()
{
    std::queue<int> empty;
    std::swap(assigned_vertexlist_index, empty);
    running = false;
    for (std::condition_variable& x : data_cond)
        x.notify_all();

    for (std::thread& thr : pool)
        thr.join();

    pool.clear();
    clear_vec(vertexlist_p_m);
    clear_vec(vertexlist_m);
    std::fill(thread_status.begin(), thread_status.end(), false);
    finished = true;
}

/* Initialization */

koch::koch(variable_wrapper& koch_wrapper)
    : iterations(0), point_counter(0),
      processor_count(std::thread::hardware_concurrency()), cos60(0.5),
      sin60(std::numbers::sqrt3 / 2), finished(true),
      stop_VBO(false), vertex_shader_paths{"./shaders/koch.vert"},
      fragment_shader_paths{"./shaders/koch.frag", "./shaders/2D_grid.frag"},
      koch_shaders(vertex_shader_paths, fragment_shader_paths),
      wrapper(koch_wrapper)
{
    wrapper.koch_mode = 0;
    wrapper.koch_next_iteration = false;
    wrapper.view_mat = glm::mat4(1.0f);
    wrapper.koch_switch_mode = false;
    wrapper.koch_grid = true;
    // glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &MAX_SHADER_STORAGE);
    vertexlist_p_s = {-150, -std::numbers::sqrt3 * 75,
                      0.0,  std::numbers::sqrt3 * 75,
                      150,  -std::numbers::sqrt3 * 75};
    vertexlist_m.resize(processor_count);
    vertexlist_p_m.resize(processor_count);
    data_cond.resize(processor_count);
    VBO_vertex_counter.resize(processor_count);
    VBO_address.resize(processor_count);
    access_opengl.resize(processor_count);
    thread_status.resize(processor_count);
    VBO_m.resize(processor_count);
    std::fill(thread_status.begin(), thread_status.end(), false);
    koch_shaders.shader_program_IDs.push_back(koch_shaders.link_shader_program(
        koch_shaders.vertex_shader_ID_list[0],
        koch_shaders.fragment_shader_ID_list[0]));
    koch_shaders.shader_program_IDs.push_back(koch_shaders.link_shader_program(
        0, koch_shaders.fragment_shader_ID_list[1]));
    gen_color_palette();

    gen_objects();
    glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
    glBufferData(GL_ARRAY_BUFFER, vertexlist_p_s.size() * sizeof(double),
                 vertexlist_p_s.data(), GL_DYNAMIC_DRAW);
}

koch::~koch()
{
    clean_multithreads();
    clean_singlethread();
    glDeleteBuffers(1, &VBO_p);
    glDeleteVertexArrays(1, &VAO_p);
    glDeleteBuffers(VBO_m.size(), VBO_m.data());
    glDeleteVertexArrays(1, &VAO_m);
}

void koch_keyboard_input(variable_wrapper* wrapper, int key, int action,
                         int mods)
{
    switch (key)
    {
        case GLFW_KEY_EQUAL:
            if (mods == GLFW_MOD_SHIFT)
            {
                // TODO WITH FOV
                /*
                wrapper->fov -= 2.0f;
                if(wrapper->fov < 1.0f)
                    wrapper->fov = 1.0f;
                */
                // wrapper->view_mat *= 2.0f;
                wrapper->view_mat = glm::scale(wrapper->view_mat,
                                               glm::vec3(1.09f, 1.09f, 1.09f));
            }
            break;

        case GLFW_KEY_KP_ADD:
            /*
            wrapper->fov -= 2.0f;
            if(wrapper->fov < 1.0f)
                wrapper->fov = 1.0f;
            */
            wrapper->view_mat =
                glm::scale(wrapper->view_mat, glm::vec3(1.09f, 1.09f, 1.09f));
            break;

        case GLFW_KEY_D:
            wrapper->view_mat = glm::mat4(1.0f);
            wrapper->fov = 45.0f;
            break;

        case GLFW_KEY_KP_SUBTRACT:
        case GLFW_KEY_MINUS:
            /*
            wrapper->fov += 2.0f;
            if(wrapper->fov > 180.0f)
                wrapper->fov = 180.0f;
            */
            wrapper->view_mat =
                glm::scale(wrapper->view_mat, glm::vec3(0.91f, 0.91f, 0.91f));
            break;

        case GLFW_KEY_RIGHT:
            wrapper->view_mat =
                glm::translate(wrapper->view_mat, glm::vec3(10.0f, 0.0f, 0.0f));
            break;

        case GLFW_KEY_LEFT:
            wrapper->view_mat = glm::translate(wrapper->view_mat,
                                               glm::vec3(-10.0f, 0.0f, 0.0f));
            break;

        case GLFW_KEY_UP:
            wrapper->view_mat =
                glm::translate(wrapper->view_mat, glm::vec3(0.0f, 10.0f, 0.0f));
            break;

        case GLFW_KEY_DOWN:
            wrapper->view_mat = glm::translate(wrapper->view_mat,
                                               glm::vec3(0.0f, -10.0f, 0.0f));
            break;

        case GLFW_KEY_N:
            if (action == GLFW_RELEASE)
                wrapper->koch_next_iteration = true;
            break;

        case GLFW_KEY_M:
            if (action == GLFW_RELEASE)
            {
                if (wrapper->koch_mode < 1)
                {
                    wrapper->koch_mode++;
                    wrapper->koch_switch_mode = true;
                }
                else
                {
                    wrapper->koch_mode = 0;
                    wrapper->koch_switch_mode = true;
                }
            }
            break;
        case GLFW_KEY_G:
            if (action == GLFW_RELEASE)
            {
                wrapper->koch_grid = !wrapper->koch_grid;
            }
            break;
        default:
            break;
    }
}

void koch_mouse_scrollback(variable_wrapper* wrapper)
{
    /*
    wrapper->fov -= y;
    if(wrapper->fov < 1.0f)
        wrapper->fov = 1.0f;
    if(wrapper->fov > 180.0f)
        wrapper->fov = 180.0f;
    */
    // std::cout << wrapper->mouse_xpos << " " << wrapper->mouse_ypos << '\n';

    float trans_x = (wrapper->mouse_xpos - (float)wrapper->window_width / 2) *
                    (wrapper->old_mouse_y_offset - wrapper->mouse_y_offset);
    float trans_y = (-wrapper->mouse_ypos + (float)wrapper->window_height / 2) *
                    (wrapper->old_mouse_y_offset - wrapper->mouse_y_offset);
    glm::vec3 mouse_center(trans_x, trans_y, 0.0f);

    if (wrapper->mouse_y_offset > wrapper->old_mouse_y_offset)
    {

        wrapper->view_mat = glm::translate(wrapper->view_mat, -mouse_center);

        wrapper->view_mat =
            glm::scale(wrapper->view_mat, glm::vec3(0.91f, 0.91f, 0.91f));

        wrapper->view_mat = glm::translate(wrapper->view_mat, mouse_center);
    }
    else
    {

        wrapper->view_mat = glm::translate(wrapper->view_mat, mouse_center);

        wrapper->view_mat =
            glm::scale(wrapper->view_mat, glm::vec3(1.09f, 1.09f, 1.09f));

        wrapper->view_mat = glm::translate(wrapper->view_mat, -mouse_center);
    }
}
