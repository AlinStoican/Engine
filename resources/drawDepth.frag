#version 330 core

layout(location = 0) out vec4 outColor;

in vec2 v_texCoords;


uniform sampler2D u_depth;

void main()
{
	//float c = texture2D(u_depth, v_texCoords).r;
	outColor.rgba = vec4(1,0,1,1);
}