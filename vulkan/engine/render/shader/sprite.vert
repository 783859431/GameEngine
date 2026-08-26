#version 450
layout(location = 0) out vec2 outUV;
layout(set = 0,binding = 0) uniform CameraMats
{
    mat4 view; 
    mat4 proj;

} cam;
vec4 verts[6] = vec4[](

    vec4(-0.5f, -0.5f, 0.0, 0.0), // 0
    vec4( 0.5f, -0.5f, 1.0, 0.0), // 1
    vec4( 0.5f,  0.5f, 1.0, 1.0), // 2
    vec4( 0.5f,  0.5f, 1.0, 1.0), // 2
    vec4(-0.5f,  0.5f, 0.0, 1.0), // 3
    vec4(-0.5f, -0.5f, 0.0, 0.0)  // 0
);

struct InstanceData {
    vec4 transform;
    vec4 rect;//采样矩形
    vec4 texture_info;//采样的图片

};

layout(set = 1, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceData;



void main() {

    vec2 localPos = verts[gl_VertexIndex].xy;
    vec2 inUV =  verts[gl_VertexIndex].zw;
    vec4 transform = instanceData.instances[gl_InstanceIndex].transform;
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
    float x = transform.x;
    float y = transform.y;
    float c = transform.z;
    float s = transform.w;
    vec2 p = vec2(info.z * localPos.x, info.w * localPos.y);
    p = vec2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    gl_Position = cam.proj * cam.view  * vec4(p,0.0f,1.0f);//  
    outUV = fragUV;
}