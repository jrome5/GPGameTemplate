#pragma once
#include <vector>
#include "physics.h"

class Particle
{
public:
	Particle(Vector3 p, float m)
	{
		position = p;
		emitter_position = p;
		mass = m;
		lifetime = physics::getRandomFloat(5.0);
	};

	~Particle() {};

	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 force;

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
	
	Vector3 calculateForce(const Vector3& acceleration)
	{
		return physics::calculateForce(acceleration, mass);
	}

	Vector3 calculateAcceleration(const Vector3& force)
	{
		return physics::calculateAcceleration(force, mass);
	}

	void calculateVelocity(const Vector3& acceleration, const float dt)
	{
		Vector3 velocity_dt = physics::calculateVelocity(acceleration, dt);
		velocity += velocity_dt;
	}

	void calculatePosition(const Vector3& velocity, const float dt)
	{
		Vector3 position_dt = physics::calculatePosition(velocity, dt);
		position += position_dt;
	}

private:
	Vector3 emitter_position;
};

class Emitter
{
public:
	Emitter(Vector3 pos)
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
	Vector3 position;
};