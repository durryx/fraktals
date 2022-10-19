#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>

#include "globals.hpp"
#include "shader.hpp"

// da rifare con array bool e funzione che controlla se lo shader è compilato

std::string shader::openfile(std::string dir)
{
    std::ifstream file;
    std::stringstream buffer;
    try
    {
        file.open(dir);
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }
    catch (const std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << '\n';
        return nullptr;
    }
}

unsigned int shader::compile_shader(std::string source, int type)
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
            // throw exection
            break;
    }
    const char* source_pointer = source.c_str();
    glShaderSource(shader, 1, &source_pointer, NULL);
    glCompileShader(shader);

    int success;
    char log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, log);
        std::cerr << "ERROR IN SHADER COMPILATION:" << type << "\n"
                  << log << '\n';
        // exception
    }
    return shader;
}

unsigned int shader::link_shader_program(unsigned int vertex = 0,
                                         unsigned int fragment = 0)
{

    // assert both shaders are not zero

    unsigned int shader_prog = glCreateProgram();
    if (vertex != 0)
        glAttachShader(shader_prog, vertex);
    if (fragment != 0)
        glAttachShader(shader_prog, fragment);
    glLinkProgram(shader_prog);

    char log[512];
    int success;
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_prog, 512, NULL, log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << log << '\n';
        // exeception
    }
    return shader_prog;
}

void shader::use(unsigned int id) { glUseProgram(id); }

shader::shader(std::vector<std::string> vertex_paths,
               std::vector<std::string> fragment_paths)
{
    for (std::string file : vertex_paths)
    {
        std::string source = openfile(file);
        unsigned int ID = compile_shader(source, 1);
        vertex_shader_ID_list.push_back(ID);
    }
    for (std::string file : fragment_paths)
    {
        std::string source = openfile(file);
        unsigned int ID = compile_shader(source, 2);
        fragment_shader_ID_list.push_back(ID);
    }
}

shader::~shader()
{
    // clean gl shader program buffer
}
