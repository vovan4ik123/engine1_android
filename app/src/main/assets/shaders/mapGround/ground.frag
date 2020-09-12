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
    //fragmentColor = texture(material.diffuseTexture, textureCoords);
    fragmentColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}