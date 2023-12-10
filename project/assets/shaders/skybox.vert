#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;
out vec3 FragPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Color = aColor;

    vec4 clipPosition = projection * view * vec4(FragPos, 1.0);
    gl_Position = clipPosition;
}