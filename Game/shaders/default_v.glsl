#version 420

// Vertex Shader Inputs
// These are the attributes of the vertex
layout(location = 0) in vec3 vIn_vertex;
layout(location = 1) in vec3 vIn_normal;
layout(location = 2) in vec3 vIn_uv;

// Uniforms
// Constants throughout the entire pipeline
// These values are sent from C++ (glSendUniform*)
uniform mat4 u_mvp;//projection mat
uniform mat4 u_mv;//view mat
uniform vec4 u_colour;

out VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vOut;

out vec3 position;

void main() 
{
	vOut.texCoord = vIn_uv;
	vOut.colour = u_colour;
	vOut.normal = (u_mv * vec4(vIn_normal, 0.0)).xyz;
	vOut.posEye = (u_mv * vec4(vIn_vertex, 1.0)).xyz;

	position = (u_mvp * vec4(vIn_vertex, 1.0)).xyz; // position of vertex in eye space
	gl_Position = u_mvp * vec4(vIn_vertex, 1.0);
}