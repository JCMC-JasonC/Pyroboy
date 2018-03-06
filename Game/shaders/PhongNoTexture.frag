#version 420

uniform vec4 lightPos;

//color
uniform vec4 objectColor;
uniform vec3 lightAmbient; //light that always affects the model
uniform vec3 lightDiffuse; //softer light
uniform vec3 lightSpecular; //shiny

//scalar
uniform float lightSpecularExponent;
uniform float attenuationConstant;
uniform float attenuationLinear;
uniform float attenuationQuadratic;

in vec3 position;
in vec3 normal;

out vec4 outColor;

void main()
{
	outColor.rgb = lightAmbient;

	//account for rasterizer interpolating
	vec3 norm= normalize(normal);

	vec3 lightVec = lightPos.xyz - position;
	float dist= length(lightVec);
	vec3 lightDir = lightVec / dist;

	float NdotL = dot(norm, lightDir);

	if(NdotL > 0.0)
	{
		//facing light

		//calculate atteniuation
		float atteuation =1.0/(attenuationConstant + (attenuationLinear *dist) + (attenuationQuadratic * dist *dist));

		//calculate diffuse
		outColor.rgb += lightDiffuse * NdotL * atteuation;

		vec3 halfVector=normalize(lightDir + normalize(-position));//doing everything in viewspace so camera is 0, making camera-position -position
		float NdotHV = max(dot(norm, halfVector),0.0);

		//calculate the specular contribution
		outColor.rgb += lightSpecular * pow(NdotHV, lightSpecularExponent) * atteuation;
	}

	outColor.rgb *= objectColor.rgb;
	outColor.a = objectColor.a;
}

