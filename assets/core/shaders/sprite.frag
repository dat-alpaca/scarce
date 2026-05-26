#version 460 core
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 v_uvs;
layout(location = 1) flat in uint v_layer;
layout(location = 3) in vec4 v_color;

layout(binding = 2) uniform sampler2DArray u_texture;

void main()
{
	vec4 color = texture(u_texture, vec3(v_uvs, float(v_layer)));
    out_color = color;
}