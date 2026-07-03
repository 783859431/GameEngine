#version 450
layout(location = 0) in vec2 inUV;
layout(set=2,binding = 0) uniform sampler2D spriteSheet;
layout(location = 0) out vec4 color;


void main() 
{
    color = texture(spriteSheet,inUV);
}
