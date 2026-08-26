#version 450
layout(location = 0) in vec2 pt1;
layout(location = 1) in vec2 pt2;
layout(location = 2) in vec2 fragPos;
layout(location = 3) in vec4 inColor;
layout(location = 4) in float thickness;
layout(location = 0) out vec4 color;
float sdLine(
    vec2 p,
    vec2 a,
    vec2 b)
{
    vec2 pa=p-a;
    vec2 ba=b-a;
    float h = clamp(dot(pa,ba)/dot(ba,ba),0.0,1.0);
    return length(pa-ba*h);
}

void main() 
{
   float d =  sdLine(fragPos,pt1,pt2);
   color = vec4(inColor.rgb,  inColor.a * (1-smoothstep(0,thickness,d)));

}
