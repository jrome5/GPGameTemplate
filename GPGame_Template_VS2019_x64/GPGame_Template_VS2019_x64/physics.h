#pragma once

constexpr glm::vec3 gravity = { 0.0f, -9.81f, 0.0f };

namespace physics
{
	float getRandomFloat(const float upper, const float offset = 0)
	{
		return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / upper)) + offset;
	}

	glm::vec3 calculateForce(const glm::vec3& acceleration, const float mass)
	{
		glm::vec3 force;
		force.x = acceleration.x * mass;
		force.y = acceleration.y * mass;
		force.z = acceleration.z * mass;
		return force;
	}

	glm::vec3 calculateAcceleration(const glm::vec3& force, const float mass)
	{
		glm::vec3 acceleration;
		acceleration.x = force.x / mass;
		acceleration.y = force.y / mass;
		acceleration.z = force.z / mass;
		return acceleration;
	}

	glm::vec3 calculateVelocity(const glm::vec3& acceleration, const float dt)
	{
		glm::vec3 velocity;
		velocity.x = acceleration.x * dt;
		velocity.y = acceleration.y * dt;
		velocity.z = acceleration.z * dt;
		return velocity;
	}

	glm::vec3 calculatePosition(const glm::vec3& velocity, const float dt)
	{
		glm::vec3 position;
		position.x = velocity.x * dt;
		position.y = velocity.y * dt;
		position.z = velocity.z * dt;
		return position;
	}
} //end namespace physics