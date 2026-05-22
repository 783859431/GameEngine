#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inColor;



layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outColor;


layout(set = 0,binding = 0) uniform CameraMats
{
    mat4 view; 
    mat4 proj;
} cam;


layout(push_constant) uniform cData{
    mat4 model;
}data;

void main() {

    gl_Position = cam.proj * cam.view * data.model * vec4(inPosition,1.0);//  
    outUV = inTexCoord;
    outColor = inColor;

}