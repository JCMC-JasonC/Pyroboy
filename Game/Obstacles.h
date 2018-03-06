#pragma once
#include"GameObject.h"

class Obstacles : public GameObject
{
public:
	Obstacles();
	~Obstacles();

	void loadMesh(const std::string &meshFile);
	void applyMesh(Mesh _mesh);
	void loadTexture(TextureType type, const std::string &texFile);

	void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection, std::vector<Light> &pointLights, Light &directionalLight);
	glm::vec3 position;

private:
};