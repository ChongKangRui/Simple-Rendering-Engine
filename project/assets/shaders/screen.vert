#version 330 core

// You should be making custom shader files instead of editing this.

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
  TexCoord = aTexCoord;
gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
