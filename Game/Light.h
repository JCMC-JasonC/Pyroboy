#pragma once
#include<glm/glm.hpp>

class Light
{
public:
	Light();
	~Light();

	glm::vec4 positionOrDirection, originalPosition;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float specularExponent;

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

};