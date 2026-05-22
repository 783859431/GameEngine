#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragViewPos;
layout(location = 3) out vec3 fragWorldPos;

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
    // 计算世界坐标位置
    vec4 worldPos = objectData.model * vec4(inPosition, 1.0);
    fragWorldPos = worldPos.xyz;
    
    // 传递纹理坐标
    fragTexCoord = inTexCoord;
    
    // 传递法线（使用模型矩阵的法线部分）
    fragNormal = mat3(objectData.model) * inNormal;
    
    // 计算视图空间位置（用于后续光照计算）
    vec4 viewPos = cam.view * worldPos;
    fragViewPos = viewPos.xyz;
    
    // 输出裁剪空间位置
    gl_Position = cam.proj * viewPos;
}