#type vertex
#version 330 core

out vec2 v_TexCoord;

void main()
{
    const vec2 positions[4] = vec2[](
        vec2(-1, -1),
        vec2(+1, -1),
        vec2(-1, +1),
        vec2(+1, +1)
    );
    const vec2 coords[4] = vec2[](
        vec2(0, 0),
        vec2(1, 0),
        vec2(0, 1),
        vec2(1, 1)
    );

    v_TexCoord = coords[gl_VertexID];
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform float bloom_spread = 0.3;
uniform float bloom_intensity = 0.7;
void main()
{

    // Temporary bloom effect, pretty much completely from https://github.com/kiwipxl/GLSL-shaders/blob/master/bloom.glsl
	ivec2 size = textureSize(u_Texture, 0);

    float uv_x = v_TexCoord.x * size.x;
    float uv_y = v_TexCoord.y * size.y;

    vec4 sum = vec4(0.0);
    for (int n = 0; n < 9; ++n) 
    {
        uv_y = (v_TexCoord.y * size.y) + (bloom_spread * float(n - 4));
        vec4 h_sum = vec4(0.0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x - (4.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x - (3.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x - (2.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x - bloom_spread, uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x, uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x + bloom_spread, uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x + (2.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x + (3.0 * bloom_spread), uv_y), 0);
        h_sum += texelFetch(u_Texture, ivec2(uv_x + (4.0 * bloom_spread), uv_y), 0);
        sum += h_sum / 9.0;
    }
    color = texture(u_Texture, v_TexCoord);//texture(u_Texture, v_TexCoord) + ((sum/9.0) * bloom_intensity);
}