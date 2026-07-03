#version 450

layout(location = 0) out vec2 f_localPos;


vec2 verts[6] = vec2[]
(
        vec2(-1,-1),
        vec2( 1,-1),
        vec2( 1, 1),
        vec2( 1, 1),
        vec2(-1, 1),
        vec2(-1,-1)
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

    vec2 localPos = verts[gl_VertexIndex];
    f_localPos = localPos;
    vec4  n_pos  =  cam.proj * cam.view * objectData.model * vec4(localPos,0,1.0);
    gl_Position = n_pos;  

}