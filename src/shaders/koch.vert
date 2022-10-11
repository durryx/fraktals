#version 460 core
layout (location = 0) in dvec2 position;

uniform mat4 transformation;

void main()
{   float x = float(position.x);
    float y = float(position.y);
    gl_Position = transformation * vec4(x, y, 0.0, 1.0);
}