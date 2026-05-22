#version 450
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;

layout( set=1 ,binding = 0) uniform sampler2D texBase;

layout(location = 0) out vec4 color;

void main() 
{
    color = texture(texBase,inUV);
}
