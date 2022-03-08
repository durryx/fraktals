#ifndef KOCH_HPP
#define KOCH_HPP

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>
#include <atomic>

#include "globals.hpp"
#include "callback_functions.hpp"
#include "shader.hpp"

void koch_keyboard_input(variable_wrapper *wrapper, int key, int action, int mods);

void koch_mouse_scrollback(variable_wrapper *wrapper);

class koch
{
    public:

        /* shaders */

        std::vector<std::string> vertex_shader_paths;
        std::vector<std::string> fragment_shader_paths;
        shader koch_shaders;

        /* various */

        variable_wrapper& wrapper;
        unsigned int iterations;
        int MAX_SHADER_STORAGE;
        const double cos60;
        const double sin60;

        /* main loop function */

        void draw_koch_snowflake(unsigned int VAO, unsigned int VBO);

        void refuse_switch(std::string current_mode, std::string last_mode);
        
        void split_drawcalls(std::vector<double> &points, bool loop);

        /* Singlethread mode */

        std::vector<double> vertexlist_s;
        std::vector<double> vertexlist_p_s;
        size_t point_counter;
        bool draw_vertexlist_s;
 
        bool insert_new_points();

        void handle_single_thread();
        
        void clean_singlethread();
        
        /* Multithreaded mode */

        unsigned int processor_count;
        std::atomic<bool> running;
        std::vector<std::thread> pool;
        std::deque<std::mutex> readvector;
        std::mutex thread_status_mutex;
        std::vector<bool> thread_status;
        std::mutex queue_mutex;
        std::deque<std::condition_variable> data_cond;
        std::queue<int> assigned_vertexlist_index;
        bool finished;

        std::vector<std::vector<double>> vertexlist_m;
        std::vector<std::vector<double>> vertexlist_m_buffer;
        std::vector<std::vector<double>> vertexlist_p_m;
        std::vector<std::array<float, 3>> colors;

        void allocate_number_of_points_memory_per_thread(std::vector<std::vector<double>>& points, int num);

        void load_points_multithread(std::vector<double> &temp);

        void gen_color_palette();

        void refill_queue();

        void init_threads();

        void infinite_loop_function(int thread_number);

        bool check_finished();
        
        void update_buffer();

        void clear_vec(std::vector<std::vector<double>>& points);

        void draw_multithread(std::vector<std::vector<double>>& points);

        void handle_multithread();

        void load_singlethread();

        void clean_multithreads();

    koch(variable_wrapper& koch_wrapper);

    ~koch();
};

#endif