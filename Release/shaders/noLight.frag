#version 420

in vec3 normal;
out vec4 outColor;
in vec2 texCoord;



struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	
	vec3 hue;

	float specularExponent;
};

uniform Material material;


void main()
{
vec2 uv = texCoord;
//uv.y = 1 - uv.y;
	vec4 diffuse = texture(material.diffuse, uv);

	outColor = diffuse;
}