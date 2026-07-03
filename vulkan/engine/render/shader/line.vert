#version 450
layout(location = 0) in vec2 inPosition;


layout(set = 0,binding = 0) uniform Matrix 
{
    mat4 view; 
    mat4 proj;
} cam; //camera Matrix 

layout(push_constant) uniform ObjectData
{
    mat4 model; //model Matrix
} objectData;

void main() {

    gl_Position = cam.proj * cam.view * objectData.model * vec4(inPosition,0,1.0);//  

}