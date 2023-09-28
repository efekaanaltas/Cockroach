#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexIndex;
in vec4 v_Color;
in vec4 v_OverlayColor;

layout (binding = 0) uniform sampler2D u_Textures[32];

vec4 lerp(vec4 a, vec4 b, float t)
{
	return a + (b-a)*t;
}

void main()
{
	color = v_Color*texture(u_Textures[int(v_TexIndex)], v_TexCoord);

	color = lerp(color, vec4(v_OverlayColor.xyz, color.w), v_OverlayColor.w);
	
	if(color.a <= 0.0) discard;
}