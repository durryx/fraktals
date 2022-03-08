#ifndef KOCH_HPP
#define KOCH_HPP

#include <vector>


void koch_draw(std::vector<float> &points1, std::vector<float> &points2, size_t &i, bool &end_of_iteration, unsigned int VAO, unsigned int VBO);
void processInput(GLFWwindow* main_window);
void split_drawcalls(std::vector<float> &points, bool end_of_iteration);
bool append_points(std::vector<float> &new_points, std::vector<float> &previous_points, size_t &i);
void koch_input(int key, int action, int mods);

struct koch_curve_data
{
    std::vector<float> points1;
    std::vector<float> points2;
    bool end_of_iteration;
    bool next_iteration;
    bool draw_p2;
    bool swap_points;
    unsigned int counter;
    unsigned int iterations;
};



#endif KOCH_HPP