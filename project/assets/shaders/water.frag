#version 330 core
//layout (location = 0) out vec4 FragColor;
//layout (location = 1) out vec4 BrightColor;
//
//
//struct PointLight{
//vec3 position;
//
//vec3 ambient;
//vec3 diffuse;
//vec3 specular;
//
//
//
//float constant;
//float linear;
//float quadratic;
//
//};
//
//struct Material{
//vec3 diffuse;
//vec3 specular;
//float shininess;
//
//
//};
//
//struct DirectionalLight{
//vec3 direction;
//vec3 ambient;
//vec3 diffuse;
//vec3 specular;
//
//
//};
//
//
//
//uniform PointLight pointLights;
//uniform DirectionalLight directionalLight;
//uniform Material material;
//uniform vec3 viewPos;
//
//in vec3 Normal;
//in vec2 TexCoord;
//in vec3 FragPos;
//
//uniform float time;
//uniform vec2 resolution;
//
//uniform sampler2D Opaquetexture;
////uniform sampler2D WaterTexture;
//uniform sampler2D DistortionTexture;
//
//
//vec3 Light(DirectionalLight light)
//{
////ambient
//    //vec3 ambient =lightColor * 0.1;
//    vec3 ambientResult = light.ambient * pointLights.ambient;
//    //diffuse
//    vec3 L = normalize(-light.direction);
//    vec3 N = normalize(Normal);
//    float diff= max(dot(L,N), 0.0);
//    vec3 diffuseResult = diff * light.diffuse * material.diffuse;
//
//    //specular
//    vec3 V = normalize(viewPos-FragPos);
//    vec3 R = reflect(-L, N);
//    
//    //float shininess = 500.0f;
//    float spec = pow(max(dot(V,R),0.0), material.shininess);
//    vec3 specularResult = spec * light.specular *material.specular;
//
//   vec3 result = ambientResult+ diffuseResult+specularResult;
//    return  result;
//}
//
//void main()
//{
//    vec2 uv = TexCoord.xy;
//     uv = uv * vec2(4,2) + vec2(0.2) * time;
//   
//  
//    vec4 distorted = texture(DistortionTexture,uv);
//
//    vec2 screenUv = gl_FragCoord.xy/resolution;
//    vec2 finaluv = screenUv + (screenUv * distorted.xy * 0.2);
//
//
//    vec3 opaqueColor = texture(Opaquetexture,finaluv).rgb;
//
//
//    FragColor = vec4( 1.0);
//
//  
//}

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
//    vec3 diffuse;
//    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
uniform vec3 viewPos;

uniform DirectionalLight directionalLight;
uniform Material material;

uniform sampler2D OpaqueTexture;
uniform sampler2D WaterTexture;
uniform sampler2D DistortTexture;
uniform vec2 resolution;
uniform float time;


vec3 directional(DirectionalLight directionalLight)
{ 
    //distortion + opaque
    vec2 uv = TexCoord.xy;
    uv = uv * vec2(4,2) + vec2 (0.2) * time;
    vec4 distorted = texture(DistortTexture, uv);
    vec2 screenUv = gl_FragCoord.xy / resolution;
    vec2 mult = screenUv * distorted.xy * 0.2;
    vec2 finaluv =  screenUv + mult.xy;
    vec3 opaquecolor = texture(OpaqueTexture, finaluv).rgb;

    vec4 textCol = texture(material.diffuse, TexCoord);
   // ambient
    vec3 ambient = directionalLight.ambient * textCol.rgb;
    
    // diffuse
    vec3 N = normalize(Normal);
    vec3 L = normalize(-directionalLight.direction);  
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * directionalLight.diffuse *  textCol.rgb ;
    diffuse+=opaquecolor;

    // specular
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);  
    float spec = pow(max(dot(V, R), 0.0), material.shininess);
    vec3 specular = directionalLight.specular * spec * texture(material.specular, TexCoord).rgb;  


    //diffuse+=opaquecolor;

    vec3 result = vec3(ambient + diffuse + specular);
    return result;
}



void main()
{
    vec3 result0 = directional(directionalLight);


    FragColor = vec4(result0, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    BrightColor = mix(vec4(0,0,0,1), vec4(result0, 1.0), brightness);

    if(brightness > 1)
        BrightColor = vec4((FragColor.rgb), 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}