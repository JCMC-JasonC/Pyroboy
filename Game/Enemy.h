#pragma once
#include "GameObject.h"
#include<glm\glm.hpp>

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(Mesh _mesh, glm::vec3 pos, glm::mat4 rot, float scale, float _speed, float health, float damage);
	~Enemy();

	void setProperty(float hp, float att);

	float getHealth();
	float getAttack();
	bool inArea(glm::vec3 &_pos, objType type);

	void update(float dt);
	void seek(glm::vec3 &pos);
	void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection, std::vector<Light> &pointLights, Light &directionalLight);
	void animate(float dt);

	void setBool(bool b);
	bool getBool()
	{
		return collided;
	}
	float speed;
	float shouldMove = true;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec3 nForce;
	std::vector<Mesh> enemyMesh;
	std::vector<Mesh> enemyAttackMesh;
	GameObject morph, attack;

	int currentFrame = 0, nextFrame = 1;
	int currentFrame1 = 0, nextFrame1 = 1;
	float m_pLocalMorphTime;

	bool isAttacking = false;

private:
	float e_health;
	float e_damage;

	//glm::vec3 nForce = glm::vec3(1.f,1.f,1.f);

	bool collided = false;

};