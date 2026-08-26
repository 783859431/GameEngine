#version 450
layout(location = 0) in vec4 v_instanceTransform;
layout(location = 1) in float v_instanceRadius;
layout(location = 2) in vec4 v_instanceColor;

layout(location = 0) out vec2 f_position;
layout(location = 1) out vec4 f_color;
layout(location = 2) out float f_thickness;
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
void main()
{
    f_position = verts[gl_VertexIndex];
    vec2 localPos  = verts[gl_VertexIndex];
    f_color = v_instanceColor;
    float radius = v_instanceRadius;

    f_thickness = 1.5f / (20 * radius);
    float x = v_instanceTransform.x;
    float y = v_instanceTransform.y;
    float c = v_instanceTransform.z;
    float s = v_instanceTransform.w;
    vec2 p = vec2(radius *localPos.x, radius * localPos.y);
    p = vec2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    gl_Position =  cam.proj * cam.view * vec4(p, 0.0f, 1.0f);
}

