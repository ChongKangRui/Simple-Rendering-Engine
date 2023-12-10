#version 330 core


layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct PointLight{
vec3 position;

vec3 ambient;
vec3 diffuse;
vec3 specular;



float constant;
float linear;
float quadratic;

};

struct Material{
sampler2D diffuse;
sampler2D specular;
float shininess;


};

struct DirectionalLight{
vec3 direction;
vec3 ambient;
vec3 diffuse;
vec3 specular;


};



uniform PointLight pointLights;
uniform DirectionalLight directionalLight;
uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;



vec3 Light(DirectionalLight light)
{
//ambient
    //vec3 ambient =lightColor * 0.1;
    vec3 ambientResult = light.ambient * pointLights.ambient;
    //diffuse
    vec3 L = normalize(-light.direction);
    vec3 N = normalize(Normal);
    float diff= max(dot(L,N), 0.0);
    vec3 diffuseResult = diff * light.diffuse * texture(material.diffuse, TexCoord).rgb;

    //specular
    vec3 V = normalize(viewPos-FragPos);
    vec3 R = reflect(-L, N);
    
    //float shininess = 500.0f;
    float spec = pow(max(dot(V,R),0.0), material.shininess);
    vec3 specularResult = spec * light.specular * texture(material.diffuse, TexCoord).rgb;

   vec3 result = ambientResult+ diffuseResult+specularResult;
    return  result;
}

void main()
{
 //vec3 textureFetch = texture(textures,TexCoord).rgb;

// textureFetch += light();
//    FragColor = vec4(textureFetch,1.0);
float threshold = 1;

vec3 result0 = Light(directionalLight);
 FragColor = vec4(result0,1.0);

 float b = dot(result0, vec3(0.299,0.587,0.114));

 float brightness;
 BrightColor = mix(vec4(0,0,0,1), vec4(result0-1.0,1.0), b>1.0);


 if(b > threshold){
 BrightColor = vec4(result0.rgb-1,1.0);
 
 }
 else{
 BrightColor = vec4(0,0,0,1);
 
 }
}