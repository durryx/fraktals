#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

class shader
{
public:
    unsigned int current_shader_ID;
    std::vector<unsigned int> vertex_shader_ID_list;
    std::vector<unsigned int> fragment_shader_ID_list;

    std::vector<unsigned int> shader_program_IDs;
    std::vector<bool> has_initialized;
    // keep track of linking initialization

    std::string openfile(std::string dir);

    unsigned int compile_shader(std::string source, int type);

    unsigned int link_shader_program(unsigned int vertex,
                                     unsigned int fragment);

    void use(unsigned int id);

    shader(std::vector<std::string> vertex_paths,
           std::vector<std::string> fragment_paths);

    ~shader();
};

#endif