#version 450
layout(location = 0) in vec2 f_pos;
layout(location = 0) out vec4 color;
layout(set = 1,binding = 0) uniform cData
{
    float radius;
    float thickness;

} cd; //camera Matrix 


void main(){

    float d = length(f_pos);
    float border_thickness = cd.thickness/cd.radius;
    float aa = fwidth(d);
    float outer =
    1.0 - smoothstep(1.0 - aa, 1.0 + aa, d);
    float inner =
    smoothstep(
        1.0 - border_thickness - aa,
        1.0 - border_thickness + aa,
        d);
    float alpha = outer * inner;
    color = vec4(1, 0, 0, alpha);
}