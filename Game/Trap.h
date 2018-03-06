#pragma once
#include"Mesh.h"
#include"ShaderProgram.h"
#include<glm/glm.hpp>
#include "Texture.h"
#include "Light.h"
#include<vector>
#include "GameObject.h"
#include<glm\glm.hpp>
class Trap: public GameObject 
{
public:
	Trap();
	Trap(std::string mesh ,std::string difTex, std::string specTex, glm::vec3 pos);
	~Trap();
	bool in_range(glm::vec3 &_pos);
	glm::vec3 trap_position;
	glm::vec4  trap_color = glm::vec4(1.f, 0.f, 0.f, 1.f);
	int numTraps = 0; 
	void draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection,
 std::vector<Light> &pointLights, Light &directionalLight);

	void setBool(bool b);
	bool getBool()
	{
		return trapbool;
	}
	
private:
	float _damage;
	bool trapbool = true;

};