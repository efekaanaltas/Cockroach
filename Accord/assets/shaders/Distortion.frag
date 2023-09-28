#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Textures[2];

void main()
{
    ivec2 size = textureSize(u_Textures[0], 0);
    vec2 texelSize = 1.0/size;

    vec2 offset = texture(u_Textures[1], v_TexCoord).xy *5*texelSize;
    color = texture(u_Textures[0], v_TexCoord + offset);
}