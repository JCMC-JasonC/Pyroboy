#pragma once

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/rotate_vector.hpp>
class Camera
{
public:
	Camera()
		: cameraPosition(15.0f),
		forwardVector(-10.0f),
		movementScalar(0.5f),
		upVector(0.0f, 1.0f, 0.0f),
		yaw(0.0f),
		pitch(0.0f),
		winWidth(1280.0f),
		winHeight(720.0f)
	{
		processMouseMotion(0, 0, 1, 1, 0.03f);
	}

	// Applies view matrix to OpenGL
	void update()
	{
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + forwardVector, upVector);
		projMatrix = glm::perspective(glm::radians(60.0f), winWidth / winHeight, 0.01f, 100.0f);

		viewProjMatrix = projMatrix * viewMatrix;
	}

	void processMouseMotion(int newX, int newY, int prevX, int prevY, float dt)
	{
		float changeX = (float)(newX - prevX);
		float changeY = (float)(newY - prevY);

		if ((abs(changeX) >= 200.0f) || (abs(changeY) >= 200.0f)) // hack to prevent jumps
			return;

		pitch = (0.08f * changeY);
		yaw = (0.08f * changeX);

		// yaw rotation
		forwardVector = glm::normalize(forwardVector);
		forwardVector = glm::rotate(forwardVector, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

		// recalculate right vector
		rightVector = glm::cross(forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
		rightVector = glm::normalize(rightVector);

		// pitch rotation
		forwardVector = glm::rotate(forwardVector, glm::radians(pitch), rightVector);

		// calc up vector
		upVector = glm::cross(rightVector, forwardVector);
		upVector = glm::normalize(upVector);
	}

	void moveUp()
	{
		cameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * movementScalar;
	}

	void moveDown()
	{
		cameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * movementScalar;
	}

	void moveForward()
	{
		cameraPosition += (forwardVector * movementScalar);
	}

	void moveBackward()
	{
		cameraPosition -= (forwardVector * movementScalar);
	}

	void moveLeft()
	{
		rightVector = glm::cross(forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
		rightVector = glm::normalize(rightVector);
		cameraPosition += (rightVector * movementScalar);
	}

	void moveRight()
	{
		rightVector = glm::cross(forwardVector, glm::vec3(0.0f, 1.0f, 0.0f));
		rightVector = glm::normalize(rightVector);
		cameraPosition -= (rightVector * movementScalar);
	}

	glm::vec3 cameraPosition;
	glm::vec3 forwardVector;
	glm::vec3 rightVector;
	glm::vec3 upVector;
	float movementScalar;
	float yaw, pitch;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::mat4 viewProjMatrix;

	float winWidth;
	float winHeight;
};