#include"Enemy.h"

Enemy::Enemy()
{
	e_damage =2.3f;
	e_health = 100.f;
	velocity = glm::vec3(10.f);

}
Enemy::Enemy(Mesh _mesh, glm::vec3 pos, glm::mat4 rot, float _scale, float _speed, float health, float damage)
{
	/*loadMesh(mesh);
	loadTexture(TextureType::Diffuse, difTexture);
	loadTexture(TextureType::Specular, specTexture);*/
	mesh =_mesh;
	position = pos;
	scale = _scale;
	speed = _speed;
	collided = false;
	e_health = health;
	e_damage = damage;
	rotate = rot;
}
Enemy::~Enemy()
{

}

void  Enemy::setProperty(float hp, float att)
{
	e_health = hp;
	e_damage = att;
}
float Enemy::getAttack()
{
	return e_damage;
}

float Enemy::getHealth()
{
	return e_health;
}
bool Enemy::inArea(glm::vec3 &_pos, objType type)
{
	glm::vec3 dist = _pos - position;
	if (type == objType::T_OBJ) {
		if (length(dist) < 12.f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	else {
		if (length(dist) < 14.f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
void Enemy::setBool(bool b)
{
	collided = b;
}

void Enemy::draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
	, std::vector<Light> &pointLights,Light &directionalLight)
{
	GameObject::draw(shader, cameraTransform,cameraProjection,pointLights, directionalLight);
}

void Enemy::update(float dt)
{
	acceleration = nForce / 1.0f;
	velocity = velocity + acceleration * dt;
	position = position + velocity * dt;
}

void Enemy::animate(float dt)
{
	m_pLocalMorphTime += dt * 50.f;

	//change frames when W/A/S/D is pressed
	if (m_pLocalMorphTime >= 1) {
		m_pLocalMorphTime = 0;

		if (nextFrame == e_mesh.size() - 1) {
			currentFrame = 0;
			nextFrame = 1;
		}
		else {
			currentFrame = nextFrame;
			nextFrame += 1;
		}
	}

	for (int i = 0; i < e_mesh[0].unpackedVertexData.size(); i++) {
		morph.mesh.unpackedVertexData[i] = Math::lerp(e_mesh[currentFrame].unpackedVertexData[i] 
			, e_mesh[nextFrame].unpackedVertexData[i], m_pLocalMorphTime);

		morph.mesh.unpackedNormalData[i] = Math::lerp(e_mesh[currentFrame].unpackedNormalData[i]
			, e_mesh[nextFrame].unpackedNormalData[i], m_pLocalMorphTime);

	}
	for (int i = 0; i < e_mesh[0].unpackedTextureData.size(); i++) {
		morph.mesh.unpackedTextureData[i] = Math::lerp(e_mesh[currentFrame].unpackedTextureData[i]
			, e_mesh[nextFrame].unpackedTextureData[i], m_pLocalMorphTime);
	}
	morph.mesh.update();
}

void Enemy::seek(glm::vec3 &pos)
{
	glm::vec3 desired = pos - position;
	direction = glm::normalize(desired);

	if (length(desired) >= length(glm::vec3(3.f)))
	{
		nForce = direction * speed;

		if (length(desired) >= length(glm::vec3(3.f)))
		{
			nForce = glm::normalize(desired) * 3.0f;
		}
		else
		{
			nForce = glm::vec3(0.f);
		}

	}
}