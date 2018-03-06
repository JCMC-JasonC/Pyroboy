#include "Trap.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>


Trap::Trap()
{
	_damage = 10.f; 
}
Trap::Trap(std::string mesh, std::string difTex, std::string specTex, glm::vec3 pos)
{
	loadMesh(mesh);
	loadTexture(TextureType::Diffuse, difTex);
	loadTexture(TextureType::Specular, specTex);
	trap_position = pos;
	trapbool = true;
	scale = 4.f;
	rotate = glm::rotate(rotate, glm::pi<float>() / 4.f, glm::vec3(0.f, 1.f, 0.f));
	rotate = glm::rotate(rotate, glm::pi<float>() / 2.f, glm::vec3(1.f, 0.f, 0.f));
	transform = translate * rotate * glm::scale(glm::mat4(), glm::vec3(scale));

}
Trap::~Trap()
{
	
}
void Trap::setBool(bool b) {
	trapbool = b;
}
bool Trap::in_range(glm::vec3 &_pos)
{
	// checks to see if any enemies are in radidus surrounding the trap
	// if so trap deletes it self and then removes health from the enemy equal to the _damage variable 
	return false; 
}

void Trap::draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
	, std::vector<Light> &pointLights, Light &directionalLight)
{
	GameObject::draw(shader, cameraTransform, cameraProjection, pointLights, directionalLight);
}

