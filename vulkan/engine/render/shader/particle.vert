#version 450
layout(location = 0) in vec2  v_instancePosition;
layout(location = 1) in float v_instanceSize;
layout(location = 2) in float v_instanceRotation;
layout(location = 3) in vec4 v_instanceColor;
vec2 verts[6] = vec2[]
(
        vec2(-1.0f,-1.0f),
        vec2( 1.0f,-1.0f),
        vec2( 1.0f, 1.0f),
        vec2( 1.0f, 1.0f),
        vec2(-1.0f, 1.0f),
        vec2(-1.0f,-1.0f)
);
layout(set = 0,binding = 0) uniform Matrix 
{
    mat4 view; 
    mat4 proj;
} cam; //camera Matrix 
layout(location = 0) out vec4 outColor;

void main()
{
    outColor =  v_instanceColor;
    vec2 localPos = verts[gl_VertexIndex];
    vec2 p = vec2(v_instanceSize *localPos.x, v_instanceSize * localPos.y);
    float c = cos(v_instanceRotation);
    float s = sin(v_instanceRotation);
    float x = v_instancePosition.x;
    float y = v_instancePosition.y;
    p = vec2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) +y);
    gl_Position =  cam.proj * cam.view * vec4(p, 0.0f, 1.0f);
}