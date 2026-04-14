#version 450
layout(location = 0) in vec2 uv;
layout(location = 1) flat in int id;
layout(location = 0) out vec4 color;
layout(set = 0, binding = 1) uniform sampler2D tex[200];

void main() {

    color = texture(tex[id],uv);

}
