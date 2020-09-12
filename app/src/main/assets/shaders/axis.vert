#version 300 es

precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

uniform mat4 MVP_matrix;

out vec3 color;

void main()
{
    color = inColor;
    gl_Position = MVP_matrix * vec4(inPosition, 1.0);
}