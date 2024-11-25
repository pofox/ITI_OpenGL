#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;

out vec4 frag_color;
void main()
{
	gl_Position = vertex_position;
	frag_color = vertex_color;
}
