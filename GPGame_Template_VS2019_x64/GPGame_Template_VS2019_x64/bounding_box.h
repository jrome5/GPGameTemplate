#pragma once
#include "shapes.h"

typedef struct {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
} Vector3;

typedef struct //physics point at centre of mass
{
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	float mass = 1.0f;
} Particle;

class BoundingBox {
public:
	BoundingBox(Vector3 position, float mass, bool show_box = true)
	{
		particle.position = position;
		particle.mass = mass;
		visual.fillColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
		visual.lineColor = glm::vec4(0.2f, 0.2, 0.2, 1.0);
		visual.lineWidth = 5.0f;
	}

	~BoundingBox() {};

	Particle particle;
	Cube visual;

	Vector3 scale = { 1.0f, 1.0f, 1.0f };


	void setScale(const float x, const float y, const float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	Vector3 getForce(const Vector3& acceleration)
	{
		Vector3 force;
		force.x = acceleration.x * particle.mass;
		force.y = acceleration.y * particle.mass;
		force.z = acceleration.z * particle.mass;
		return force;
	}

	Vector3 getAcceleration(const Vector3& force)
	{
		Vector3 acceleration;
		acceleration.x = force.x / particle.mass;
		acceleration.y = force.y / particle.mass;
		acceleration.z = force.z / particle.mass;
		return acceleration;
	}

	void getVelocity(const Vector3& acceleration, const float dt)
	{
		particle.velocity.x += acceleration.x * dt;
		particle.velocity.y += acceleration.y * dt;
		particle.velocity.z += acceleration.z * dt;
	}

	void getPosition(const Vector3& velocity, const float dt)
	{
		particle.position.x += velocity.x * dt;
		particle.position.y += velocity.y * dt;
		particle.position.z += velocity.z * dt;
	}
};