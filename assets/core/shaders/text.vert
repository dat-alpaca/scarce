#version 460 core
layout(location = 0) out vec2 v_uvs;
layout(location = 1) flat out uint v_layer;
layout(location = 2) flat out uint v_paint_background;
layout(location = 3) out vec4 v_color;
layout(location = 4) out vec4 v_background_color;

struct Character
{
	vec2 position;
	vec2 uvSize;
	uint layer;
	uint paintBackground;
	vec4 color;
	vec4 backgroundColor;
};

layout(std430, binding = 0) restrict readonly buffer Characters
{
	Character characters[];
} b_characters;

layout(std140, binding = 1) uniform World
{
	mat4 model;
	mat4 projection;
} u_world;

vec2 c_quad_vertex[6] = vec2[](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),

	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0)
);

void main()
{
	uint currentQuad = gl_VertexID / 6;
	uint currentVertex = gl_VertexID % 6;

	Character currentCharacter = b_characters.characters[currentQuad];

	vec2 localPos = currentCharacter.position + vec2(c_quad_vertex[currentVertex]);

	gl_Position = u_world.projection * u_world.model * vec4(localPos, 0.0, 1.0);
	v_layer = currentCharacter.layer;
	v_uvs = c_quad_vertex[currentVertex] * currentCharacter.uvSize;
	v_color = currentCharacter.color;
	v_background_color = currentCharacter.backgroundColor;
	v_paint_background = currentCharacter.paintBackground;
}