#pragma once

#include "VertexBufferObject.h"
#include "glm/vec3.hpp"
#include "Camera.h"
#include <memory>
#include"Texture.h"
#include <math.h>
#include "ShaderProgram.h"

class ParticleEmitterSoA
{
	/*
	Theory time!
	Reminder: Particle emitters are responsible for creating (emitting), updating, killing and respawning individual particles.
	The particle emitter also stores the data for each particle. Traditionally we would create a Particle structure which contains
	the data for the particles, something like this:

	struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		//...
	}


	We would then create an array of this structure by doing something like:
	Particle* particles = new Particles[N]

	To access each particle individually we would do something like:
	particles[i].position = ... such that i < N

	This is known as an object centric design and is commonly referred to as an Array of Structures (AoS).

	An alternative way of storing the data is by using a property centric design known as a Structure of Arrays (SoA).
	
	This is how we will be storing the data in this class. The idea is that we have a single structure, which contains
	a bunch of arrays, one for each attribute of the particle (see below).

	What's the point of doing this?
	Since we are updating our particles on the CPU and sending the particle positions to the 
	GPU every frame, we want to minimize the amount of data we are transferring. By storing our data in a SoA
	we are able to only send the data we care about to the GPU, in this case we only want to send the particle positions.
	So we could keep our data as an AoS, but we'd be wasting bandwidth because we're sending irrelevant data.

	Yay!
	*/
	struct Particle
	{
		glm::vec3* positions;
		glm::vec3* velocities;
		glm::vec3* accelerations;
		float*	remainingLives;
		float* masses;
		// Other particle properties...
	} particles;

public:
	ParticleEmitterSoA();
	~ParticleEmitterSoA();

	// Initialize memory for particles
	void initialize(unsigned int numParticles);

	// playback control
	inline void play() { playing = true; }
	inline void pause() { playing = false; }

	// Updates each particle in system
	void update(float dt);

	// Draws particles to screen
	void draw(glm::mat4 uModel, glm::mat4 &uView, glm::mat4 &uProj);

	void freeMemory();

	// Emitter position in world space
	// aka where the particles spawn
	glm::vec3 initialPosition;

	// locomotion properties
	// Ranges store the min (x) and max(y) bound of random values for the property
	glm::vec3 lifeRange;
	glm::vec3 initialForceMin;
	glm::vec3 initialForceMax;


	VertexBufferObject vbo;

	bool playing;	// false is update is paused

	std::shared_ptr<ShaderProgram> material;

	std::shared_ptr<Texture> texture;
private:
	unsigned int numParticles; // Number of particles passed into initialize()
	bool allocated; // false if memory not allocated
};