#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

float normalizeRange(float x, float min, float max)
{
    return (x - min) / (max-min);
}

float easeOut(float x)
{
    return 1 - (1 - x) * (1 - x);
}

float easeIn(float x)
{
    return x*x;
}

void simple()
{
    vec3 ceil = vec3(0.1);
    vec3 center = vec3(0.15);
    vec3 floor = vec3(0.15);

    // some hack to make it "radial"
    vec4 clipPos =vec4 (projection * vec4(FragPos, 1.0));
    vec4 norm = normalize(clipPos);

    float t = clamp(normalizeRange(norm.y, -1.0, 1.0), 0.0, 1.0);

    vec3 finalColor = t < 0.5 ? 
    mix(floor, center, easeIn(clamp(normalizeRange(t,0.0, 0.5), 0.0,1.0))):
    mix(center, ceil, easeOut(clamp(normalizeRange(t,0.5, 1.0), 0.0,1.0)));

    FragColor = vec4(finalColor, 1.0);
}

void main()
{    
    simple();
}