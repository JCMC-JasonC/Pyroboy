#include "Obstacles.h"
#include<iostream>

Obstacles::Obstacles()
{

}
Obstacles::~Obstacles()
{

}
void Obstacles::applyMesh(Mesh _mesh)
{
	mesh = _mesh;
}
void Obstacles::loadMesh(const std::string &meshFile)
{
	GameObject::loadMesh(meshFile);
}
void Obstacles::loadTexture(TextureType type, const std::string &texFile)
{
	GameObject::loadTexture(type,texFile);
}

void Obstacles::draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
	, std::vector<Light> &pointLights, Light &directionalLight)
{
	GameObject::draw(shader, cameraTransform, cameraProjection, pointLights, directionalLight);
}