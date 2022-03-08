#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "koch.hpp"
#include "globals.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


double currentFrame, deltatime;
double lastFrame = 0;
int MAX_SHADER_STORAGE;
int fractal = 0;
glm::mat4 projection = glm::mat4(1.0f);



void callback_resize_func(GLFWwindow* main_window, int WIDTH, int HEIGHT)
{
    glViewport(0, 0, WIDTH, HEIGHT);
}

void key_callback(GLFWwindow* main_window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(main_window, true);
        break;

    case GLFW_KEY_EQUAL:
        if(mods == GLFW_MOD_SHIFT)
        {
            projection = glm::scale(projection, glm::vec3(1.05f, 1.05f, 0.0f));
        }
        break;
    case GLFW_KEY_KP_ADD:
        projection = glm::scale(projection, glm::vec3(1.05f, 1.05f, 0.0f));
        break;

    case GLFW_KEY_KP_SUBTRACT:
    case GLFW_KEY_MINUS:
        projection = glm::scale(projection, glm::vec3(0.95f, 0.95f, 0.0f));
        break;
        
    case GLFW_KEY_RIGHT:
        projection = glm::translate(projection, glm::vec3(0.01f, 0.0f, 0.0f));
        break;
    
    case GLFW_KEY_LEFT:
        projection = glm::translate(projection, glm::vec3(-0.01f, 0.0f, 0.0f));
        break;

    case GLFW_KEY_UP:
        projection = glm::translate(projection, glm::vec3(0.0f, 0.01f, 0.0f));
        break;  

    case GLFW_KEY_DOWN:
        projection = glm::translate(projection, glm::vec3(0.0f, -0.01f, 0.0f));
        break;

    default:
        break;
    }

    switch (fractal)
    {
    case 0:
        koch_input(key, action, mods);
        break;
    
    default:
        break;
    }
}

unsigned int compile_shader(std::string source, int type)
{
    unsigned int shader;
    switch (type)
    {
    case 1:
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
    case 2:
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    default:
        exit(-1);
    }
    const char *source_pointer = source.c_str();
    glShaderSource(shader, 1, &source_pointer, NULL);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cout << "ERROR IN SHADER COMPILATION\n" << log << std::endl;
        exit(-1);
    }
    return shader;
}

std::string load_shader_source(std::string file_name)
{
    std::ifstream t(file_name);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}


int main(int argc, char** argv){
     
    // add command line arguments

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* main_window = glfwCreateWindow(WIDTH, HEIGHT, "fraktals", NULL, NULL);
    if (main_window == NULL)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(main_window);

    glfwSetFramebufferSizeCallback(main_window, callback_resize_func);

    glfwSetKeyCallback(main_window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        exit(-1);
    }
    
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &MAX_SHADER_STORAGE);
    //std::cout << MAX_SHADER_STORAGE << std::endl;






    std::string source = load_shader_source("../scr/shaders/koch.vert");
    unsigned int vertexshader = compile_shader(source, 1);
    source = load_shader_source("../scr/shaders/koch.frag");
    unsigned int fragmentshader = compile_shader(source, 2);

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertexshader);
    glAttachShader(shader, fragmentshader);
    glLinkProgram(shader);

    char log[512];
    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
    glGetProgramInfoLog(shader, 512, NULL, log);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << log << std::endl;
    exit(-1);
	}

    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    struct koch_curve_data koch_data = {
        {-0.5f, -sqrt3_2/2, 0.0f, sqrt3_2/2, 0.5f, -sqrt3_2/2},
        {},
        true,
        true,
        true,
        false,
        0,
        0
    };
    
    //bool flush_opengl_mem = true;
    //unsigned int max_iterations = (std::log(MAX_SHADER_STORAGE/3) - std::log(2))/(2 * std::log(2));
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points1.size() * sizeof(float), points1.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




    while(!glfwWindowShouldClose(main_window))
    {
        currentFrame = glfwGetTime();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //  add selector for various fractals
        fractal = 0;




        glUseProgram(shader);

        unsigned int tranformationLoc= glGetUniformLocation(shader, "transform");
        glUniformMatrix4fv(tranformationLoc, 1, GL_FALSE, glm::value_ptr(projection));

        koch_draw(points1, points2, counter1, end_of_iteration, VAO, VBO);


        glfwSwapBuffers(main_window);
        glfwPollEvents();
        lastFrame = currentFrame;
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader);

    glfwTerminate();

    return 0;
}
