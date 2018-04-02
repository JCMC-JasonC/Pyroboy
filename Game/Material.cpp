#include "Material.h"
#include<iostream>

Material::Material()
{

}
Material::Material(const std::string &diffusePath, const std::string &specularPath, float specExp, glm::vec3 &hue)
	:specularExponent(specExp), hue(hue)
{
	loadTexture(TextureType::Diffuse, diffusePath);
	loadTexture(TextureType::Specular, specularPath);
	
}
Material::~Material()
{

}

void Material::loadTexture(TextureType type, const std::string &texFile)
{
	switch (type)
	{
	case Diffuse:
		diffuse.load(texFile);
		break;
	case Specular:
		specular.load(texFile);
		break;
	case Normal:				/////////////
		normal.load(texFile);	/////////////
		break;
	default:
	{
		std::cout << "Error - TextureType not found: " << type << std::endl;
		system("pause");
		exit(0);
	}
		break;
	}
}