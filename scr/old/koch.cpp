#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "koch.hpp"
#include "globals.hpp"


void koch_input(int key, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_N:
        if (action == GLFW_RELEASE)
        {
            NEXT = true;
            SWAP[1] = !SWAP[1];
        }
        break;
    
    default:
        break;
    }
}

bool append_points(std::vector<float> &new_points, std::vector<float> &previous_points, unsigned int &i)
{ 
    size_t len = previous_points.size();
    float dx, dy;
    
    if (i != (len - 2))
    {
        dx = previous_points[i+2] - previous_points[i];
        dy = previous_points[i+3] - previous_points[i+1];        
    } else
    {
        dx = previous_points[0] - previous_points[i];
        dy = previous_points[1] - previous_points[i+1];
    }

    std::array<float, 8> temp = 
    {
        previous_points[i],
        previous_points[i+1],

        previous_points[i] + dx/3,
        previous_points[i+1] + dy/3,

        previous_points[i] + dx/2 - dy*sin60/3,
        previous_points[i+1] + dy/2 + dx*sin60/3,

        previous_points[i] + 2*dx/3,
        previous_points[i+1] + 2*dy/3
    };

    /*
    if (new_points.size() < MAX_SHADER_STORAGE/sizeof(float))
    {
        glBufferSubData(GL_ARRAY_BUFFER, new_points.size() * sizeof(float), sizeof(float) * temp.size(), temp.data());
    }
    */

    new_points.insert(new_points.end(), temp.begin(), temp.end());

    if(i == (len - 2))
    {
        return true;
    } else
    {
        i+=2;
        return false;
    }
}

void split_drawcalls(std::vector<float> &points, bool end_of_iteration)
{
    for(size_t j = 0, i = MAX_SHADER_STORAGE/sizeof(float); j < points.size(); j+=MAX_SHADER_STORAGE/sizeof(float), i+=MAX_SHADER_STORAGE/sizeof(float))
    {
        if(i <= points.size())
        {
            std::vector<float> temp = {points.begin() + j, points.begin() + i};
            glBufferData(GL_ARRAY_BUFFER, MAX_SHADER_STORAGE, temp.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_LINE_STRIP, 0, temp.size()/2);
        } else
        {
            if(end_of_iteration)
            {
                glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINE_LOOP, 0, points.size()/2);
            } else
           
                glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINE_STRIP, 0, points.size()/2);
            }
        }
    }
}

void koch_draw(koch_curve_data &DATA, unsigned int VAO, unsigned int VBO)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    /*
    if (SWAP[0] == SWAP[1] && flush_opengl_mem)
    {
        glInvalidateBufferData(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, MAX_SHADER_STORAGE, NULL, GL_DYNAMIC_DRAW);
        flush_opengl_mem = false;
    }
    */

    if(DATA.next_iteration)
    {
        if(DATA.swap_points)
        {
            DATA.end_of_iteration = append_points(DATA.points2, DATA.points1, DATA.counter);
            if (DATA.end_of_iteration)
            {
                //std::cout << DATA.points2.size() << std::endl;
                DATA.counter = 0;
                DATA.points1 = {};
                DATA.iterations++;
                DATA.next_iteration = false;
                DATA.swap_points = !DATA.swap_points;
            }
        } else
        {
            DATA.end_of_iteration = append_points(DATA.points1, DATA.points2, DATA.counter);
            if (DATA.end_of_iteration)
            {
                //std::cout << DATA.points1.size() << std::endl;
                DATA.counter = 0;
                DATA.points2 = {};
                DATA.iterations++;
                DATA.next_iteration = false;
                DATA.swap_points = !DATA.swap_points;
            }
        }
    }

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    if(DATA.draw_p2)
    {
        split_drawcalls(points2, end_of_iteration);
    } else
    {
        split_drawcalls(points1, end_of_iteration);
    }
    
}
