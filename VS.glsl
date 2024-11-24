#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;

uniform float theta;

out vec4 frag_color;
void main()
{
	//gl_Position = vertex_position;

	float x = vertex_position.x;
	float y = 0.5*sin((x+theta) * 3.14 * 36)*sin((x+theta)*3.14*3);
	
	gl_Position = vec4(x,y,vertex_position.z,1);

	//frag_color = vertex_position;
	frag_color = vertex_color;
}