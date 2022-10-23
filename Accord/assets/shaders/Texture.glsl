#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in float a_TexIndex;
layout(location = 3) in vec4 a_OverlayColor;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out float v_TexIndex;
out vec4 v_OverlayColor;

void main()
{
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_OverlayColor = a_OverlayColor;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexIndex;
in vec4 v_OverlayColor;

layout (binding = 0) uniform sampler2D u_Textures[32];

vec4 lerp(vec4 a, vec4 b, float t)
{
	return a + (b-a)*t;
}

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord);

	color = lerp(color, vec4(v_OverlayColor.xyz, color.w), v_OverlayColor.w);
}