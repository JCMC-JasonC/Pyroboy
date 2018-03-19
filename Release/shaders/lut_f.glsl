#version 420

uniform vec4 u_lightPos;
uniform vec4 u_colour;

// Note: Uniform bindings
// This lets you specify the texture unit directly in the shader!
layout(binding = 0) uniform sampler3D u_rgb; // rgb texture

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

// Multiple render targets!
layout(location = 0) out vec4 FragColor;
in vec3 position;
in vec4 fragcolor;

vec3 diffuse()
{
	vec3 L = normalize(u_lightPos.xyz - vIn.posEye);
	vec3 N = normalize(vIn.normal);

	float ndotl = max(0.0, dot(N, L));
	vec3 diffuseColor = texture(u_rgb, vIn.texCoord).rgb;

	if (length(diffuseColor) == 0)
		diffuseColor = vec3(0.5);

	vec3 diffuse = texture(u_rgb, vec3(ndotl)).rgb;

	return diffuse;
}


vec3 specular()
{
	vec3 L = normalize(u_lightPos.xyz - vIn.posEye);
	vec3 N = normalize(vIn.normal);
	
	vec3 R = reflect(L, N);
	float VdotR = max(dot(normalize(-vIn.posEye), R), 0.0);
	vec3 specular = texture(u_rgb, vec3(pow(VdotR, 1.0f))).rgb;

	return specular;
}
void main()
{
	vec3 lightVec = u_lightPos.xyz - vIn.posEye;
	float dist= length(lightVec);
	float atteuation =1.0/(1.0f + (0.1f *dist) + (0.01f * dist *dist));

	vec3 ambient = vec3(atteuation);
	FragColor = vec4(ambient + (diffuse() + specular()), 1.0);
}