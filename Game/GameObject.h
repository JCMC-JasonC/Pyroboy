#pragma once
#include"Mesh.h"
#include"ShaderProgram.h"
#include<glm/glm.hpp>
#include "Material.h"
#include "Light.h"
#include<vector>
#include <memory>

enum uiType {
	PLAYER,
	TREE
};

class GameObject 
{
public:

	typedef std::shared_ptr<GameObject> Ptr;

	GameObject();
	~GameObject();

	Mesh mesh;
	Material mat;
	Texture texture;

	glm::vec3 position;
	glm::vec3 originalPosition;
	glm::vec3 direction;

	glm::mat4 originalTranslate;
	glm::mat4 originalTransform;
	glm::mat4 translate;
	glm::mat4 rotate;
	float scale = 1.f;
	glm::mat4 transform;

	float rotX;
	float rotY;
	float rotZ;

	
	bool shouldDraw = true; // Used for UI elements to decide which ones to draw

	virtual void loadMesh(const std::string &meshFile);
	virtual void loadTexture(TextureType type, const std::string &texFile);
	virtual void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
		,std::vector<Light> &pointLights, Light &directionalLight);

};
