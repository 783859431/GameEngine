#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;


layout(location = 0) out vec2 outUV;


layout(set = 0,binding = 0) uniform Matrix 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} mats;

layout(set = 0,binding = 1) uniform Data
{
   vec2 viewport;

} data;



void main() {

    gl_Position =  mats.proj * mats.view * mats.model * vec4(inPosition,0.0,1.0);
    outUV = inTexCoord;

}