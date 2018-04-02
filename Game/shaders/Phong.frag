#version 420
#define NUM_POINT_LIGHTS 1

struct PointLight
{
	vec4 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;


	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct DirectionalLight
{
	vec4 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	
	vec3 hue;

	float specularExponent;
};

uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight directionalLight;
uniform Material material;

in vec3 position;
in vec2 texCoord;
in vec3 normal;

out vec4 outColor;

vec3 calculatePointLight(PointLight light, vec3 norm, vec4 materialDiffuse, vec4 materialSpecular);

vec3 calculateDirectionalLight(DirectionalLight light, vec3 norm, vec4 materialDiffuse, vec4 materialSpecular);

void main()
{
	//account for rasterizer interpolating
	vec3 norm= normalize(normal);
	vec4 diffuse = texture(material.diffuse, texCoord);
	vec4 specular = texture(material.specular, texCoord);

	outColor.rgb = calculateDirectionalLight(directionalLight, norm, diffuse, specular);

	
	for(int i=0; i<NUM_POINT_LIGHTS; i++)
	{
		outColor.rgb += calculatePointLight(pointLights[i], norm, diffuse, specular);
	}

	outColor.rgb *=material.hue;
	outColor.a = diffuse.a;
}

vec3 calculatePointLight(PointLight light, vec3 norm,  vec4 materialDiffuse, vec4 materialSpecular)
{
	vec3 lightVec = light.position.xyz - position;
	float dist= length(lightVec);
	vec3 lightDir = lightVec / dist;


	//facing light

	//calculate atteniuation
	float atteuation =1.0/(light.constantAttenuation + (light.linearAttenuation *dist) + (light.quadraticAttenuation * dist *dist));

	vec3 ambient = atteuation * light.ambient;

	//calculate diffuse
	float NdotL = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * NdotL * atteuation *materialDiffuse.rgb;

	vec3 reflectDir = reflect(-lightDir, norm);
	float VdotR = max(dot(normalize(-position), reflectDir), 0.0);
	vec3 specular = pow(VdotR, material.specularExponent) * atteuation * light.specular * materialSpecular.rgb;
	
	return ambient + diffuse + specular;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 norm, vec4 materialDiffuse, vec4 materialSpecular)
{
	vec3 lightDir = normalize(-light.direction).xyz;

	vec3 ambient =  light.ambient;

	//calculate diffuse
	float NdotL = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * NdotL  *materialDiffuse.rgb;

	vec3 reflectDir = reflect(-lightDir, norm);
	float VdotR = max(dot(normalize(-position), reflectDir), 0.0);
	vec3 specular = pow(VdotR, material.specularExponent)  * light.specular * materialSpecular.rgb;
	
	return ambient + diffuse + specular;
}