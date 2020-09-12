#version 300 es

precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTextureCoords;

out vec2 textureCoords;

uniform mat4 P_matrix;

void main()
{
    textureCoords = inTextureCoords;

    gl_Position = P_matrix * vec4(inPosition, 1.0f);
}