#pragma once
#include<glm/glm.hpp>
#include"Texture.h"

class Material
{
public:

	Material();
	Material(const std::string &diffusePath, const std::string &specularPath, float specExp = 50.f, glm::vec3 &hue = glm::vec3(1.f));
	~Material();

	void loadTexture(TextureType type, const std::string &texFile);


	Texture diffuse;
	Texture specular;
	Texture normal;
	glm::vec3 hue = glm::vec3(1.f);
	float specularExponent = 50.f;
};