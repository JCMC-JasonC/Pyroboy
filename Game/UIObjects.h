#pragma once
#include"GameObject.h"

class UIGameObjects : public GameObject
{
public:

	UIGameObjects();
	UIGameObjects(Mesh _mesh);
	~UIGameObjects();

	void loadMesh(const std::string &meshFile);
	void loadTexture(TextureType type, const std::string &texFile);

	void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection, std::vector<Light> &pointLights, Light &directionalLight);
	glm::vec3 position;
	bool active = true;
private:
};