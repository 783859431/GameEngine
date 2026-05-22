#version 450
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec4 color;

void main() 
{
    color = vec4(inColor,1);
}
