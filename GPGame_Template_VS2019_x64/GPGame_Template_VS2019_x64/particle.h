#pragma once
#include <vector>
#include "physics.h"
#include <glm/glm.hpp>

typedef struct Particle
{
	Particle(const glm::vec3 p, float m)
	{
		position = p;
		emitter_position = p;
		mass = m;
		lifetime = physics::getRandomFloat(10.0);
		velocity = glm::vec3(0.0f, 1.0f, 0.0f);
		trans = 1.0f;
	};

	float mass = 1.0f;
	float lifetime = 0.0f;
	float age = 0.0f;
	float trans = 1.0f;

	glm::vec3 emitter_position;
	glm::vec3 position;
	glm::vec3 velocity;
	

	bool checkExpired(const float dt)
	{
		age += dt;
		if (lifetime < age)
		{
			lifetime = physics::getRandomFloat(10.0);
			age = 0.0f;
			position = emitter_position;
			velocity = { 0.0, 1.0f, 0.0f };
			return true;
		}
		return false;
	}

	void resetPosition()
	{
		position = emitter_position;
		velocity = glm::vec3(0.0f, 1.0f, 0.0f);
		trans = 1.0f;                              // Is this needed ???
		return;
	}

	float getRandColor() 
	{

	}
};

class Emitter
{
public:
	Emitter()
	{
	};

	~Emitter() {};

	void spawn(const glm::vec3& pos, const int n)
	{
		number_of_particles = n;
		position = pos;
		for (int i = 0; i < number_of_particles; i++)
		{
			Particle p(pos, 0.0f);
			particles.push_back(p);
		}
	}

	Particle& getParticle(const int i)
	{
		return particles[i];
	}

	glm::vec3 getPosition()
	{
		return position;
	}
	
private:
	int number_of_particles  = 0;
	std::vector<Particle> particles;
	glm::vec3 position;
};