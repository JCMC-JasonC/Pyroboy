#pragma once
#include"GameObject.h"

class Bullet : public GameObject
{
public:

	Bullet();
	Bullet(Mesh mesh,glm::vec3 pos, float scale, float _speed, float damage, glm::vec3 direction);
	~Bullet();

	void update(float dt);
	void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection,
		std::vector<Light> &pointLights, Light &directionalLight);
	
	float getDamage() { return _damage; }

	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 nForce;

	float speed;
	bool active;
	
private:
	float _damage;
};
