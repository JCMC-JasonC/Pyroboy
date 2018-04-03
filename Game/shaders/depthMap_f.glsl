#version 420
out vec4 FragColor;

in vec2 TexCoords;

uniform float near_plane;
uniform float far_plane; 

layout(binding = 0) uniform sampler2D depthMap;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}
void main()
{
	//gl_FragDepth = gl_FragCoord.z;
	float depthValue = texture(depthMap, TexCoords).r;
	//FragColor = vec4(vec3(LinearlizeDepth(depthValue) / far_plane), 1.0); //perspective
	FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}