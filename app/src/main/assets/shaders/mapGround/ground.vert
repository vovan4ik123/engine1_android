#version 300 es

precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoords;

out vec2 textureCoords;

uniform mat4 MVP_matrix;

void main()
{
    textureCoords = inTextureCoords;
    //normal =
    //fragPos =
    gl_Position = MVP_matrix * vec4(inPosition, 1.0f);
}