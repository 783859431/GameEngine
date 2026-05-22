#version 450

// 输入布局
layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragViewPos;
layout(location = 3) in vec3 fragWorldPos;

// 输出
layout(location = 0) out vec4 outColor;

// Uniform 缓冲区 - 材质属性
layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4 color;           // RGBA 颜色
    float metallic;       // 金属度 (0-1)
    float roughness;      // 粗糙度 (0-1)
    float padding[2];     // 内存对齐
} material;

// Uniform 缓冲区 - 光照
layout(set = 1, binding = 1) uniform LightUBO {
    vec3 lightPos;        // 光源位置
    vec3 lightColor;      // 光源颜色
    float lightIntensity; // 光源强度
} light;

// 纹理采样器
layout(set = 2, binding = 0) uniform sampler2D albedoMap;      // 基础颜色贴图
layout(set = 2, binding = 1) uniform sampler2D normalMap;    // 金属度贴图
//layout(set = 1, binding = 2) uniform sampler2D roughnessMap;   // 粗糙度贴图

// 常量
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// 法线分布函数 (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

// ----------------------------------------------------------------------------
// 几何遮蔽函数 (Smith-Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------
// 菲涅尔函数 (Fresnel-Schlick)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ----------------------------------------------------------------------------
void main() {
    // 从贴图采样材质属性
    vec4 albedo = texture(albedoMap, fragTexCoord);
    float metallic = 1;
    float roughness = 1;
    
    // 应用 Uniform 中的材质属性覆盖
    albedo *= material.color;
    metallic *= material.metallic;
    roughness *= material.roughness;
    
    // 输入向量
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(-fragViewPos);          // 观察方向（指向相机）
    vec3 L = normalize(light.lightPos - fragWorldPos);  // 光源方向
    vec3 H = normalize(V + L);                  // 半角向量
    
    // 计算反射率 F0（非金属使用 0.04，金属使用 albedo 颜色）
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);
    
    // 反射率方程
    vec3 Lo = vec3(0.0);
    
    // 计算每个光源的贡献（这里使用单个光源）
    float NdotL = max(dot(N, L), 0.0);
    
    if (NdotL > 0.0) {
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL;
        vec3 specular = numerator / max(denominator, 0.001);
        
        // 漫反射贡献
        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        
        // 光照贡献
        float attenuation = light.lightIntensity / (distance(light.lightPos, fragWorldPos) * distance(light.lightPos, fragWorldPos));
        vec3 radiance = light.lightColor * attenuation;
        
        Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
    }
    
    // 环境光照（简单的固定环境色）
    vec3 ambient = albedo.rgb * 0.05;
    
    // 最终颜色
    vec3 finalColor = ambient + Lo;
    
    // Gamma 校正
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    // 输出颜色（使用材质 Alpha）
    outColor = vec4(finalColor, albedo.a);
}