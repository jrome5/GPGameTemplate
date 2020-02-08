#pragma once
#include <vector>
#include "physics.h"

class Particle
{
public:
	Particle(Vector3 p, float m)
	{
		position = p;
		mass = m;
	};

	~Particle() {};

	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	float mass = 1.0f;

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
};

class Emitter
{
	Emitter() {};
	~Emitter() {};

	int number_of_particles = 100;
	std::vector<Particle> particles;
};