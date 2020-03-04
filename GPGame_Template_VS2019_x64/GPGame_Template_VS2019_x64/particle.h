#pragma once
#include <vector>
#include "physics.h"

class Particle
{
public:
	Particle(glm::vec3 p, float m)
	{
		position = p;
		emitter_position = p;
		mass = m;
		lifetime = physics::getRandomFloat(5.0);
	};

	~Particle() {};

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;

	float mass = 1.0f;
	float lifetime = 0.0f;
	float age = 0.0f;

	bool checkExpired(const float dt)
	{
		age += dt;
		if (lifetime < age)
		{
			lifetime = physics::getRandomFloat(5.0);
			age = 0.0f;
			position = emitter_position;
			velocity = { 0.0, 0.0f, 0.0f };
			force = { 0.0, 0.0f, 0.0f };
			acceleration = { 0.0, 0.0f, 0.0f };
			return true;
		}
		return false;
	}
	
	glm::vec3 calculateForce(const glm::vec3& acceleration)
	{
		return physics::calculateForce(acceleration, mass);
	}

	glm::vec3 calculateAcceleration(const glm::vec3& force)
	{
		return physics::calculateAcceleration(force, mass);
	}

	void calculateVelocity(const glm::vec3& acceleration, const float dt)
	{
		glm::vec3 velocity_dt = physics::calculateVelocity(acceleration, dt);
		velocity += velocity_dt;
	}

	void calculatePosition(const glm::vec3& velocity, const float dt)
	{
		glm::vec3 position_dt = physics::calculatePosition(velocity, dt);
		position += position_dt;
	}

private:
	glm::vec3 emitter_position;
};

class Emitter
{
public:
	Emitter(glm::vec3 pos)
	{
		position = pos;
		for (int i = 0; i < number_of_particles; i++)
		{
			Particle p(pos, 0.0f);
			particles.push_back(p);
		}
	};

	~Emitter() {};

	int number_of_particles = 100;
	std::vector<Particle> particles;
	glm::vec3 position;
};