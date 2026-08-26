#version 450
//layout(location = 0) in vec2 f_pos;
//layout(location = 0) out vec4 color;
//layout(set = 1,binding = 0) uniform cData
//{
//    float radius;
//    float thickness;
//
//} cd; //camera Matrix 
//
//
//void main(){
//
//    float d = length(f_pos);
//    float border_thickness = cd.thickness/cd.radius;
//    float aa = fwidth(d);
//    float outer =
//    1.0 - smoothstep(1.0 - aa, 1.0 + aa, d);
//    float inner =
//    smoothstep(
//        1.0 - border_thickness - aa,
//        1.0 - border_thickness + aa,
//        d);
//    float alpha = outer * inner;
//    color = vec4(1, 0, 0, alpha);
//}
//
layout(location = 0) in vec2 f_position;
layout(location = 1) in vec4 f_color;
layout(location = 2) in float f_thickness;

layout(location = 0) out vec4 fragColor;



vec4 blend_colors(vec4 front, vec4 back)
{
    vec3 cSrc = front.rgb;
    float alphaSrc = front.a;
    vec3 cDst = back.rgb;
    float alphaDst = back.a;

    vec3 cOut = cSrc * alphaSrc + cDst * alphaDst * (1.0 - alphaSrc);
    float alphaOut = alphaSrc + alphaDst * (1.0 - alphaSrc);
    cOut = cOut / alphaOut;

    return vec4(cOut, alphaOut);
}

void main()
{
    // radius in unit quad
    float radius = 0.9;

    // distance to axis line segment
    vec2 e = vec2(radius, 0);
    vec2 w = f_position;
    float we = dot(w, e);
    vec2 b = w - e * clamp(we / dot(e, e), 0.0, 1.0);
    float da = length(b);

    // distance to circle
    float dw = length(w);
    float dc = abs(dw - radius);

    // union of circle and axis
    float d = min(da, dc);

    vec4 borderColor = f_color;
    vec4 fillColor = 0.6f * borderColor;

    // roll the fill alpha down at the border
    vec4 back = vec4(fillColor.rgb, fillColor.a * smoothstep(radius + f_thickness, radius, dw));

    // roll the border alpha down from 1 to 0 across the border thickness
    vec4 front = vec4(borderColor.rgb, smoothstep(f_thickness, 0.0f, d));

    fragColor = blend_colors(front, back);
}
