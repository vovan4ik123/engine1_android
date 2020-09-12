#version 300 es

precision mediump float;

in vec2 textureCoords;

out vec4 fragmentColor;

uniform sampler2D texture1;

void main()
{
	fragmentColor = texture(texture1, textureCoords);
}