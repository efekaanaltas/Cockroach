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
uniform float brightnessThreshold;

float brightness(vec3 color)
{
    return max(color.r, max(color.g, color.b));
}

void main()
{
    vec4 texCol = texture(u_Texture, v_TexCoord);

    color = vec4(texCol.rgb*clamp(((brightness(texCol.rgb)-brightnessThreshold)/(1-brightnessThreshold)), 0.0, 1.0), texCol.a);

    //if(brightness(texCol.rgb) >= brightnessThreshold)
    //    color = texCol;
    //else
    //    color = vec4(0.0,0.0,0.0,1.0);
}