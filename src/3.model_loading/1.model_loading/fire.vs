#version 410 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
//uniform bool pFixed;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;
layout(location = 1) in vec4 colour;

out Vertex	{
	vec2 texCoord;
	vec4 colour;
} OUT;

void main(void)	{
	gl_Position = (projMatrix) * viewMatrix * modelMatrix * vec4(position, 1.0);
	OUT.texCoord = texCoord;
	//OUT.colour = vec4(1.0,1.0,1.0,1.0);
	OUT.colour	= colour;
}