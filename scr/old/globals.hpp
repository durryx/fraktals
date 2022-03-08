#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const auto PI = std::acos(-1);
const auto cos60 = std::cos(PI/3);
const auto sin60 = std::sin(PI/3);
const float sqrt3_2 = sin60;
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

extern bool NEXT;
extern bool SWAP[];
extern unsigned int iterations;

extern double currentFrame;
extern double lastFrame;
extern int MAX_SHADER_STORAGE;
extern int fractal;
extern glm::mat4 projection;
