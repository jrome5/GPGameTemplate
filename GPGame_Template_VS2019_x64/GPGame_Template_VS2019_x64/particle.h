#pragma once
#include <vector>
#include "physics.h"

class Particle
{
public:
	Particle(const glm::vec3& p, float m)
	{
		position = p;
		emitter_position = p;
		mass = m;
		lifetime = physics::getRandomFloat(5.0);
	};

	~Particle() {};

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
	
	glm::vec3 getPosition() const
	{
		return position;
	}

	glm::vec3 getVelocity() const
	{
		return velocity;
	}

	void resetPosition()
	{
		position = emitter_position;
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		return;
	}

private:
	glm::vec3 emitter_position;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;
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