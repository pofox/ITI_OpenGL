#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;

out vec4 frag_color;
void main()
{
	gl_Position = vertex_position;
	if(vertex_color.a==1)
	{
		frag_color = vertex_color;
	}
	else
	{
		frag_color=vec4(0,0.5,0.5,1);
	}
	
}