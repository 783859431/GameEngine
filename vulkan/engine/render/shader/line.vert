#version 450
layout(location = 0) in vec2 point1;
layout(location = 1) in vec2 point2;
layout(location = 2) in vec4 lineColor;

layout(location = 0) out vec2 outP1;
layout(location = 1) out vec2 outP2;
layout(location = 2) out vec2 fragPos;
layout(location = 3) out vec4 outColor;
layout(location = 4) out float thickness;


vec2 verts[6] = vec2[](

    vec2(-1.0, -1.0), // 0
    vec2( 1.0, -1.0), // 1
    vec2( 1.0,  1.0), // 2
    vec2( 1.0,  1.0), // 2
    vec2(-1.0,  1.0), // 3
    vec2(-1.0, -1.0)  // 0
);

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
    fragPos = verts[gl_VertexIndex];
    outColor = lineColor;


    float w = abs( point1.x- point2.x);
    float h = abs( point1.y- point2.y);
    vec2 center = 0.5 * (point1 + point2);
    float maxWidth = max(w,h);//等到包围盒长边
    float scale =  0.5 * maxWidth;
    float invScale = 1 / scale;

    outP1 = invScale * (point1 - center);
    outP2 = invScale * (point2 - center);


    float pixelScale = 20.0f;
    thickness = 2.0f / pixelScale * invScale;
    vec2 p = vec2(scale * verts[gl_VertexIndex].x, scale * verts[gl_VertexIndex].y) + center;
    gl_Position = cam.proj * cam.view  * vec4(p,0,1.0);//  

}