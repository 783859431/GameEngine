#version 450
layout(location = 0) in vec2 vWorldPos;
layout(location = 1) in vec2 v2;
layout(location = 0) out vec4 outColor;
float gridLine(float coord, float scale)
{
    float v = coord * scale;
    float d = abs(fract(v - 0.5) - 0.5);
    float w = fwidth(v);
    return 1.0 - smoothstep(0.0, w, d);
}

float f(float x){

  return cos(x);
}
float f2(float x){
return sin(x);
}

float functionLine(vec2 world)
{
    float x = world.x;
    float y = world.y;

    float dx = fwidth(x);

    float y0 = f(x - dx);
    float y1 = f(x + dx);

    // 判断像素 y 是否被函数段穿过
    float ymin = min(y0, y1);
    float ymax = max(y0, y1);

    float w = fwidth(y) * 1.5;

    return 1.0 - smoothstep(0.0, w,
        max(ymin - y, y - ymax)
    );
}
float functionLine2(vec2 world)
{
    float x = world.x;
    float y = world.y;

    // 计算一个像素对应的 world delta
    float dx = fwidth(x);

    // 采样区间两端
    float y0 = f2(x - dx);
    float y1 = f2(x + dx);

    // 处理无效点（避免 NaN/Inf）
   // if (!isnan(y0) || !isnan(y1)) return 0.0;

    // 极端变化检测（渐近线自动断开）


    // 区间覆盖检测
    float ymin = min(y0, y1);
    float ymax = max(y0, y1);

    //自适应线宽（像素空间）
    float w = max(fwidth(y) * 1.5, 0.5 / v2.y);

    //计算距离并返回 alpha
    float d = max(ymin - y, y - ymax);
    return 1.0 - smoothstep(0.0, w, d);
}

void main()
{
    // ----------------------
    // 计算屏幕像素每单位世界长度
    // ----------------------
       // 3. pixelsPerUnit
    float pixelsPerUnit = v2.x / (2.0 * v2.y);

    float v1 = functionLine(vWorldPos);
    float v2 = functionLine2(vWorldPos);
    // 4. 动态网格间距
    float spacing = 1.0;
    while (spacing * pixelsPerUnit < 50.0) spacing *= 2.0;
    while (spacing * pixelsPerUnit > 100.0) spacing /= 2.0;
 
    float minor = max(
     gridLine(vWorldPos.x, spacing / 4.0),
     gridLine(vWorldPos.y, spacing / 4.0)
       );

    float major = max(
    gridLine(vWorldPos.x, spacing/16),
    gridLine(vWorldPos.y, spacing/16)
       );



float axisX = 1.0 - smoothstep(
    0.0,
    fwidth(vWorldPos.y) * 1.5,
    abs(vWorldPos.y)
);

float axisY = 1.0 - smoothstep(
    0.0,
    fwidth(vWorldPos.x) * 1.5,
    abs(vWorldPos.x)
);

float axis = max(axisX, axisY);
    vec3 color = vec3(1.0); // 白底

// 次网格
color = mix(color, vec3(0.5), minor);

// 主网格
color = mix(color, vec3(0.1), major);

// 坐标轴
color = mix(color, vec3(0,1,0), axis);

// 函数曲线（最上层）

color = mix(color, vec3(1.0, 0.0, 0.0), v1);

color = mix(color, vec3(1.0, 0.0, 0.0), v2);

outColor = vec4(color, 1.0);

}