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

void main()
{
	//ivec2 size = textureSize(u_Texture, 0);

    //float uv_x = v_TexCoord.x * size.x;
    //float uv_y = v_TexCoord.y * size.y;

    color = texture(u_Texture, v_TexCoord);
}