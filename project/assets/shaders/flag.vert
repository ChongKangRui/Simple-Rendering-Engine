#version 330 core

// You should be making custom shader files instead of editing this.

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    vec3 pos = FragPos;

     vec2 v = pos.xz - vec2(1,1);
     float dist = length(v);
     float freq = 1.5; 
     float amp = 0.2 * max(2-dist,0);

    pos.z = sin(dist * time * freq) * amp;

    gl_Position = projection * view * model * vec4(pos, 1.0f);
}
