#version 420
layout (location = 0) in vec3 in_vert;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_normal;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal; 
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec3 viewPos;

void main()
{
	viewPos = vec3(view * vec4(in_vert, 1.0));
	vs_out.FragPos = vec3(model * vec4(in_vert, 1.0));
	vs_out.Normal = transpose(inverse(mat3(model))) * in_normal;
	vs_out.TexCoords = in_uv;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position = projection * view * model * vec4(in_vert, 1.0);
}