#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform float brightnessThreshold;

float brightness(vec3 color)
{
    return max(color.r, max(color.g, color.b));
}

vec4 prefilter(vec4 color)
{
    float multiplier = (brightness(color.rgb)-brightnessThreshold)/(1-brightnessThreshold);
    multiplier = clamp(multiplier, 0.0, 1.0);
    return vec4(color.rgb*multiplier, color.a);
}

void main()
{
    ivec2 size = textureSize(u_Texture, 0);

    vec2 texelSize = 1.0/size;

    vec4 A = texture(u_Texture, v_TexCoord + texelSize * vec2(-1.0, -1.0));
    vec4 B = texture(u_Texture, v_TexCoord + texelSize * vec2( 0.0, -1.0));
    vec4 C = texture(u_Texture, v_TexCoord + texelSize * vec2( 1.0, -1.0));
    vec4 D = texture(u_Texture, v_TexCoord + texelSize * vec2(-0.5, -0.5));
    vec4 E = texture(u_Texture, v_TexCoord + texelSize * vec2( 0.5, -0.5));
    vec4 F = texture(u_Texture, v_TexCoord + texelSize * vec2(-1.0,  0.0));
    vec4 G = texture(u_Texture, v_TexCoord                               );
    vec4 H = texture(u_Texture, v_TexCoord + texelSize * vec2( 1.0,  0.0));
    vec4 I = texture(u_Texture, v_TexCoord + texelSize * vec2(-0.5,  0.5));
    vec4 J = texture(u_Texture, v_TexCoord + texelSize * vec2( 0.5,  0.5));
    vec4 K = texture(u_Texture, v_TexCoord + texelSize * vec2(-1.0,  1.0));
    vec4 L = texture(u_Texture, v_TexCoord + texelSize * vec2( 0.0,  1.0));
    vec4 M = texture(u_Texture, v_TexCoord + texelSize * vec2( 1.0,  1.0));

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4 o = (D + E + I + J) * div.x;
    o += (A + B + G + F) * div.y;
    o += (B + C + H + G) * div.y;
    o += (F + G + L + K) * div.y;
    o += (G + H + M + L) * div.y;

    color = prefilter(o);
}