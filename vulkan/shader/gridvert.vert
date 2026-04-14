#version 450
layout(location = 0) out vec2 vWorldPos;
layout(location = 1) flat out vec2 v2;
layout(push_constant) uniform PC
{
    vec4 camPos_zoom;
    mat4 mvp;
    vec2 viewport; 

} pc;

vec2 pos[6] = vec2[](
    vec2(-1,-1), vec2( 1,-1), vec2( 1, 1),
    vec2(-1,-1), vec2( 1, 1), vec2(-1, 1)
);

void main()
{
    gl_Position = vec4(pos[gl_VertexIndex], 0, 1);
    vec2 screen = (pos[gl_VertexIndex]  + 1) *0.5* pc.viewport;
    vWorldPos = (screen - pc.viewport * 0.5) / pc.camPos_zoom.z + pc.camPos_zoom.xy;//ÆÁÄ»×ø±êÖÐÐÄ
    v2 = vec2(pc.viewport.y,pc.camPos_zoom.z);

}
