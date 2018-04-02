#include"Player.h"
#include"AnimationMath.h"

Player::Player()
{

}
Player::~Player()
{

}
void Player::shoot()
{

}
void Player::drawWithShadows(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
	, glm::mat4 &lightSpaceMatrix, bool isShadowMap)
{
	morph.transform = transform;
	morph.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
}
void Player::draw(ShaderProgram &shader, glm::mat4 &cameraTransform, glm::mat4 &cameraProjection
	, std::vector<Light> &pointLights, Light &directionalLight)
{
	//GameObject::draw(shader, cameraTransform, cameraProjection, pointLights, directionalLight);
	morph.transform = transform;
	morph.draw(shader, cameraTransform, cameraProjection, pointLights, directionalLight);
}
void Player::update(float dt)
{

	acceleration = nForce / 1.0f;
	velocity = velocity + acceleration * dt;
	position = position + velocity * dt;

	nForce = glm::vec3(0.f);
}
void Player::animate(float dt)
{
	m_pLocalMorphTime += dt *50.f;

	//change frames when W/A/S/D is pressed
	if (m_pLocalMorphTime >= 1) {
		m_pLocalMorphTime = 0;

		if (nextFrame == playerMesh.size() - 1) {
			currentFrame = 0;
			nextFrame = 1;
		}
		else {
			currentFrame = nextFrame;
			nextFrame += 1;
		}
	}

	for (int i = 0; i < playerMesh[0].unpackedVertexData.size(); i++) {
		morph.mesh.unpackedVertexData[i] = Math::lerp(playerMesh[currentFrame].unpackedVertexData[i]
			, playerMesh[nextFrame].unpackedVertexData[i], m_pLocalMorphTime);

		morph.mesh.unpackedNormalData[i] = Math::lerp(playerMesh[currentFrame].unpackedNormalData[i]
			, playerMesh[nextFrame].unpackedNormalData[i], m_pLocalMorphTime);

	}
	for (int i = 0; i < playerMesh[0].unpackedTextureData.size(); i++) {
		morph.mesh.unpackedTextureData[i] = Math::lerp(playerMesh[currentFrame].unpackedTextureData[i]
			, playerMesh[nextFrame].unpackedTextureData[i], m_pLocalMorphTime);
	}
	morph.mesh.update();
}