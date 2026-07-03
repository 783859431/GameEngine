#version 450
layout(location = 0) out vec2 outUV;
layout(location = 1)  flat out int texId;
layout(set = 0,binding = 0) uniform CameraMats
{
    mat4 view; 
    mat4 proj;

} cam;
vec4 verts[6] = vec4[](

    vec4(-0.5, -0.5, 0.0, 0.0), // 0
    vec4( 0.5, -0.5, 1.0, 0.0), // 1
    vec4( 0.5,  0.5, 1.0, 1.0), // 2
    vec4( 0.5,  0.5, 1.0, 1.0), // 2
    vec4(-0.5,  0.5, 0.0, 1.0), // 3
    vec4(-0.5, -0.5, 0.0, 0.0)  // 0
);

struct InstanceData {
    mat4 model;
    vec4 rect;
    vec4 texture_info;
};

layout(set = 1, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceData;



void main() {

    vec2 inPos = verts[gl_VertexIndex].xy;
    vec2 inUV =  verts[gl_VertexIndex].zw;
    mat4 model = instanceData.instances[gl_InstanceIndex].model;
    vec4 rect = instanceData.instances[gl_InstanceIndex].rect;
    vec4 info = instanceData.instances[gl_InstanceIndex].texture_info;
    
    vec2 uvMin = vec2(
    rect.x/info.x,
    rect.y/info.y);
    vec2 uvMax = vec2(
    (rect.x + rect.z)/info.x,
    (rect.y + rect.w)/info.y);

    vec2 fragUV = mix(
        uvMin,
        uvMax,
        inUV
    );

    gl_Position = cam.proj * cam.view * model * vec4(inPos,0,1.0);//  
    outUV = fragUV;
}