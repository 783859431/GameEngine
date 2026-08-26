#version 450
vec2 verts[6] = vec2[](

    vec2(-1.0, -1.0), // 0
    vec2( 1.0, -1.0), // 1
    vec2( 1.0,  1.0), // 2
    vec2( 1.0,  1.0), // 2
    vec2(-1.0,  1.0), // 3
    vec2(-1.0, -1.0)  // 0
);

layout(location = 0) in vec4 v_instanceTransform; //
layout(location = 1) in vec4 v_instancePoints12;  ////多边形的点
layout(location = 2) in vec4 v_instancePoints34;  //
layout(location = 3) in vec4 v_instancePoints56;  //
layout(location = 4) in vec4 v_instancePoints78;  //
layout(location = 5) in vec4 v_instanceColor;     //
layout(location = 6) in int v_instanceCount;      ////多边形的顶点数量要小于等于八个
layout(location = 7) in float v_instanceRadius;   //
layout(set = 0,binding = 0) uniform Matrix 
{
    mat4 view; 
    mat4 proj;
} cam; //camera Matrix 


 layout(location = 0) out vec2 f_position;
 layout(location = 1) out vec4 f_color;
 layout(location = 2) out vec2 f_points[8];
 layout(location = 10) flat out int f_count;
 layout(location = 11) out float f_radius;
 layout(location = 12) out float f_thickness;

void main() {

    f_position = verts[gl_VertexIndex];
    f_color = v_instanceColor;

    f_radius = v_instanceRadius;
    f_count = v_instanceCount;

    f_points[0] = v_instancePoints12.xy;
    f_points[1] = v_instancePoints12.zw;
    f_points[2] = v_instancePoints34.xy;
    f_points[3] = v_instancePoints34.zw;
    f_points[4] = v_instancePoints56.xy;
    f_points[5] = v_instancePoints56.zw;
    f_points[6] = v_instancePoints78.xy;
    f_points[7] = v_instancePoints78.zw;

    // Compute polygon AABB 计算包围盒，这个包围盒就是最小的能够容纳这个图形的最小矩形
    vec2 lower = f_points[0];
    vec2 upper = f_points[0];
    for (int i = 1; i < v_instanceCount; ++i)
    {
        lower = min(lower, f_points[i]);
        upper = max(upper, f_points[i]);
    }

    vec2 center = 0.5 * (lower + upper); //等到了包围盒的中心点坐标
    vec2 width = upper - lower; //包围盒的边
    float maxWidth = max(width.x, width.y);//等到包围盒长边

    float scale = f_radius + 0.5 * maxWidth;
    float invScale = 1 / scale;

    // Shift and scale polygon points so they fit in 2x2 quad
    for (int i = 0; i < f_count; ++i)
    {
        f_points[i] = invScale * (f_points[i] - center);
    }

    // Scale radius as well
    f_radius = invScale * f_radius;

    // resolution.y = pixelScale * scale
    float pixelScale = 20.0f;
    f_thickness = 2.0f / pixelScale * invScale;

    // scale up and transform quad to fit polygon
    float x = v_instanceTransform.x;
    float y = v_instanceTransform.y;
    float c = v_instanceTransform.z;
    float s = v_instanceTransform.w;


    vec2 p = vec2(scale * verts[gl_VertexIndex].x, scale * verts[gl_VertexIndex].y) + center;
    p = vec2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    gl_Position = cam.proj * cam.view * vec4(p, 0.0f, 1.0f);

  //  gl_Position = cam.proj * cam.view * objectData.model * vec4(inPosition,0,1.0);//  

}