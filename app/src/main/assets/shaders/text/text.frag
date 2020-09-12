#version 300 es

precision mediump float;

in vec2 textureCoords;

out vec4 fragmentColor;

uniform sampler2D text;
uniform vec3 color;
uniform float transparency;

void main()
{
    // all data stored in R component in texture
    float colorValue = texture(text, textureCoords).r;
    if(colorValue <= 0.1f) discard; // discard ALL full transparent pixel !!!
    fragmentColor = vec4(color, (colorValue * transparency));
}