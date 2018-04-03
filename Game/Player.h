#pragma once
#include"GameObject.h"
#include"Bullet.h"

class Player : public GameObject
{
public:
	Player();
	~Player();

	void shoot();
	void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection, std::vector<Light> &pointLights, Light &directionalLight);
	void drawWithShadows(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
		, glm::mat4 &lightSpaceMatrix, bool isShadowMap);
	void update(float dt);
	void animate(float dt);
	int getCurrentFrame() { return currentFrame; }

	int currentFrame=0, nextFrame=1;
	float speed, m_pLocalMorphTime;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 nForce;

	std::vector<Bullet*>bullets;
	std::vector<Mesh> playerMesh;
	GameObject morph;
private:
	float _health;
};