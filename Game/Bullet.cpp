#include"Bullet.h"
#include<glm/gtc/matrix_transform.hpp>

Bullet::Bullet()
{

}
Bullet::~Bullet()
{

}
Bullet::Bullet(Mesh _mesh,glm::vec3 pos, float scale, float _speed, float damage, glm::vec3 dir)
{
	/*loadMesh(mesh);
	loadTexture(TextureType::Diffuse, diffTexture);
	loadTexture(TextureType::Specular, specTexture);*/
	mesh = _mesh;
	position = pos;
	
	translate = glm::translate(translate, position);
	transform = translate * rotate * glm::scale(glm::mat4(), glm::vec3(scale));
	speed = _speed;
	direction = dir;
	_damage = damage;
}
void Bullet::update(float dt)
{
	//acceleration = nForce / 1.0f;
	velocity = direction * speed * dt;
	position = position + velocity;

	//nForce = glm::vec3(0.f);
}
void Bullet::draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection,
	std::vector<Light> &pointLights, Light &directionalLight)
{
	GameObject::draw(shader, cameraTransform, cameraProjection, pointLights, directionalLight);
}