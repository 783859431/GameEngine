#version 450

layout(location = 0) in vec4 v_instanceRect;     //字大小
layout(location = 1) in vec4 v_instanceSampleRect;  //采样的位置
layout(location = 2) in vec4 v_instanceColor;


layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

layout(set = 0,binding = 0) uniform Matrix 
{
    mat4 view; 
    mat4 proj;
} cam; //camera Matrix 

layout(set = 1,binding = 0) uniform TextureInfo
{
   float w;
   float h;
} texInfo;
vec4 verts[6] = vec4[](

    vec4(-0.5f, -0.5f, 0.0, 0.0), // 0
    vec4( 0.5f, -0.5f, 1.0, 0.0), // 1
    vec4( 0.5f,  0.5f, 1.0, 1.0), // 2
    vec4( 0.5f,  0.5f, 1.0, 1.0), // 2
    vec4(-0.5f,  0.5f, 0.0, 1.0), // 3
    vec4(-0.5f, -0.5f, 0.0, 0.0)  // 0
);
void main()
{
    vec2 localPos = verts[gl_VertexIndex].xy;
    vec2 inUV = verts[gl_VertexIndex].zw;
    localPos+= vec2(0.5,0.5);
    vec2 p = vec2(localPos.x * v_instanceRect.z+v_instanceRect.x, 
    localPos.y * v_instanceRect.w+v_instanceRect.y);
    vec2 uvMin = vec2(
    v_instanceSampleRect.x/texInfo.w,
    v_instanceSampleRect.y/texInfo.h);
    vec2 uvMax = vec2(
    (v_instanceSampleRect.x + v_instanceSampleRect.z)/texInfo.w,
    (v_instanceSampleRect.y + v_instanceSampleRect.w)/texInfo.h);

    vec2 fragUV = mix(
        uvMin,
        uvMax,
        inUV
    );
    gl_Position = cam.proj  * vec4(p,0,1.0);//  
    outUV = fragUV;
    outColor = v_instanceColor;

}