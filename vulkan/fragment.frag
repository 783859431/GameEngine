#version 450
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 color;
layout(set = 0, binding = 2) uniform sampler2D tex[2];

void main() 
{
    color = texture(tex[0], uv);
}
