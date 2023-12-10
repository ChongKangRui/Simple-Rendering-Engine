#version 330 core

// DO NOT EDIT

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 colour;

void main()
{
    FragColor = BrightColor = vec4(colour, 1.0);
}