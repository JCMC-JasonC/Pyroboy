#include"UIObjects.h"

UIGameObjects::UIGameObjects()
{

}
UIGameObjects::UIGameObjects(Mesh _mesh)
{
	mesh = _mesh;
	
}
UIGameObjects::~UIGameObjects()
{

}

void UIGameObjects::loadMesh(const std::string &meshFile)
{
	GameObject::loadMesh(meshFile);
}
void UIGameObjects::loadTexture(TextureType type, const std::string &texFile)
{
	GameObject::loadTexture(type, texFile);
}

void UIGameObjects::draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
	, std::vector<Light> &pointLights, Light &directionalLight)
{
	GameObject::draw(shader, cameraTransform, cameraProjection, pointLights, directionalLight);
}