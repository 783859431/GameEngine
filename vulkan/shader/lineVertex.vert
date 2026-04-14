#version 450
layout(location = 0) in vec2 inPos;
layout(location = 1) in int inSide;
layout(location = 0) out vec4 outColor;


//layout(push_constant) uniform TIME {
//   float deltaTime;//每帧的时间
//   float time;     // 时间戳
//} Time;


//layout(binding = 0) uniform UniformBufferObject 
//{
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo;

layout (push_constant) uniform LineInfo
{
   float lineWidth;
   vec4 lineColor;
   vec2 viewport;
   vec2 dir;
}line;


void main() {
  vec4 clip = vec4(inPos, 0, 1);

  // NDC 偏移
  vec2 offset = vec2(-line.dir.y, line.dir.x);
  offset = normalize(offset);
  offset *= inSide * line.lineWidth/line.viewport * 2;
  clip.xy += offset;

  gl_Position = clip;
  outColor =  line.lineColor;



}
