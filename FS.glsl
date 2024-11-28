#version 330

in vec4 frag_color;

void main()
{
	if(abs(frag_color.x)<=0.15&&abs(frag_color.z)<=0.15)
	{
		gl_FragColor = vec4(0.5f,0.0f,0.0f,1);
	}
	else{
		gl_FragColor = vec4(0.0f,0.5f,0.0f,frag_color.y>=0.2?1:frag_color.y<=-0.5?0:(frag_color.y+0.5)/0.7);
	}
}