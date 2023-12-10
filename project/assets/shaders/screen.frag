#version 330 core

// You should be making custom shader files instead of editing this.
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;


uniform sampler2D colorAttachment0;
uniform sampler2D colorAttachment1;
uniform sampler2D blurAttachment;
uniform bool CameraInWater;

void main()
{

int state = 3;
vec4 color = vec4(0,0,0,1);


switch(state)
{
case 0:{
    color = texture(blurAttachment,TexCoord);
    break;

}

case 1:{
    color = texture(colorAttachment0,TexCoord);
    break;
}
case 2:{
    color = texture(colorAttachment1,TexCoord);
    break;

}
case 3:{
    color = texture(colorAttachment0,TexCoord);
    vec4 color1 = texture(blurAttachment,TexCoord);

color.rgb += color1.rgb;
float originalB = color.b;

if(CameraInWater){
color.g+=0.3;
color.b+=0.7;
}
else{
color.b=originalB;

}

break;

}

}

 
FragColor = color;
    
}