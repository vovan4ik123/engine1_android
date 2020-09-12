#version 300 es

precision mediump float;

in vec2 textureCoords;

out vec4 fragmentColor;

struct Material
{
    sampler2D diffuseTexture;
    sampler2D specularTexture;
};

uniform Material material;

void main()
{
    fragmentColor = texture(material.diffuseTexture, textureCoords);
}