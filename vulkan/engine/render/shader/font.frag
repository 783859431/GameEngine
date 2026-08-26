#version 450
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 1) uniform sampler2D textureAtlas;  
void main() 
{
    //  SDF
	//	outColor = vec4(inColor.xyz*dist, inColor.w*dist);
	//	outColor = inColor;
	float d = texture(textureAtlas, inUV).r;
    float aaf = fwidth(d);
	float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
	outColor = vec4(inColor.rgb, alpha);
}
