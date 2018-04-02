#version 420
layout (location = 0) in vec3 in_vert;
layout (location = 1) in vec2 in_uv;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 TexCoords;

void main()
{	
	TexCoords = in_uv;
	gl_Position = lightSpaceMatrix * model * vec4(in_vert,1.0);
}