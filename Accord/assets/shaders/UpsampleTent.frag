#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Textures[2];

void main()
{
    ivec2 size = textureSize(u_Textures[0], 0);
    vec2 texelSize = 1.0/size;

    float sampleScale = 1.0;
    vec4 d = texelSize.xyxy * vec4(1.0, 1.0, -1.0, 0.0) * sampleScale;

    vec4 s;
    s =  texture(u_Textures[0], v_TexCoord - d.xy);
    s += texture(u_Textures[0], v_TexCoord - d.wy) * 2.0;
    s += texture(u_Textures[0], v_TexCoord - d.zy);

    s += texture(u_Textures[0], v_TexCoord + d.zw) * 2.0;
    s += texture(u_Textures[0], v_TexCoord       ) * 4.0;
    s += texture(u_Textures[0], v_TexCoord + d.xw) * 2.0;

    s += texture(u_Textures[0], v_TexCoord + d.zy);
    s += texture(u_Textures[0], v_TexCoord + d.wy) * 2.0;
    s += texture(u_Textures[0], v_TexCoord + d.xy);

    color = s * (1.0 / 16.0) + texture(u_Textures[1], v_TexCoord);
}