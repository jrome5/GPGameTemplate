#pragma once
typedef struct {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
} Vector3;

void operator+=(Vector3& v1, Vector3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

void operator+=(Vector3& v1, const Vector3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

constexpr Vector3 gravity = { 0.0f, -9.81f, 0.0f };

namespace physics
{
	float getRandomFloat(const float upper, const float offset = 0)
	{
		return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / upper)) + offset;
	}

	Vector3 calculateForce(const Vector3& acceleration, const float mass)
	{
		Vector3 force;
		force.x = acceleration.x * mass;
		force.y = acceleration.y * mass;
		force.z = acceleration.z * mass;
		return force;
	}

	Vector3 calculateAcceleration(const Vector3& force, const float mass)
	{
		Vector3 acceleration;
		acceleration.x = force.x / mass;
		acceleration.y = force.y / mass;
		acceleration.z = force.z / mass;
		return acceleration;
	}

	Vector3 calculateVelocity(const Vector3& acceleration, const float dt)
	{
		Vector3 velocity;
		velocity.x = acceleration.x * dt;
		velocity.y = acceleration.y * dt;
		velocity.z = acceleration.z * dt;
		return velocity;
	}

	Vector3 calculatePosition(const Vector3& velocity, const float dt)
	{
		Vector3 position;
		position.x = velocity.x * dt;
		position.y = velocity.y * dt;
		position.z = velocity.z * dt;
		return position;
	}
} //end namespace physics