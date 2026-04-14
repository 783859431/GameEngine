#version 450
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 0) out vec2 outUV;
layout(location = 1) out int outIndex;
layout(push_constant) uniform UniformBufferObject 
{
   vec2 viewport;
   int index;
} pc;

layout(set=0,binding = 0)uniform Matrix 
{
   mat4 model;
   mat4 view;
   mat4 proj;
} u1;

void main() 
{
    vec4 world = u1.model * vec4(inPosition,0.0,1.0);
    vec4 clip  = u1.proj * u1.view * world;
    gl_Position = clip;
    outUV = inUV; 
    outIndex = pc.index;
}