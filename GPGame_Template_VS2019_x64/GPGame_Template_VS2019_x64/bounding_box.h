#pragma once
#include "shapes.h"
#include "physics.h"
#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

inline glm::vec3 mulVector(glm::vec3 v1, float m)
{
	glm::vec3 v2;
	v2.x = v1.x * m;
	v2.y = v1.y * m;
	v2.z = v1.z * m;
	return v2;
}

inline glm::vec3 limitVector(glm::vec3 v1, float m)
{
	glm::vec3 v2;
	v2.x = (abs(v1.x) < m) ? 0.0f : v1.x;
	v2.y = (abs(v1.y) < m) ? 0.0f : v1.y;
	v2.z = (abs(v1.z) < m) ? 0.0f : v1.z;
	return v2;
}

class BoundingBox
{
public:
	BoundingBox(const glm::vec3& pos, const float m, const int identifier= -1)
	{
		position = pos;
		mass = m;
		velocity = { 0.0f, 0.0f, 0.0f };
		visual.fillColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
		visual.lineColor = glm::vec4(0.2f, 0.2, 0.2, 1.0);
		visual.lineWidth = 5.0f;
		aabb.c = glm::vec3{ position.x, position.y, position.z };
		aabb.r = glm::vec3{ 1.0f, 1.0f, 1.0f };
		id = identifier;
	}

	~BoundingBox() {};

	Cube visual;
	AABB aabb;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	float mass;
	float friction = 0.98f;
	bool is_static = false;
	bool at_rest = false;
	int id;

	void setScale(const float x, const float y, const float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
		aabb.r = glm::vec3{ x / 2, y / 2, z / 2 };
	}

	void addForce(const glm::vec3 force)
	{
		acceleration = physics::calculateAcceleration(force, mass);
	}

	void setPosition(const glm::vec3& pos)
	{
		aabb.c = pos;
	}

	void calculateResponse(BoundingBox& b2)
	{
		const float velocity_cutoff = 0.05f; //stop vibration due to bouncing at low velocity
		const auto m1 = mass;
		const auto m2 = b2.mass;
		auto u1 = velocity;
		auto u2 = b2.velocity;
		if (not is_static)
		{
			auto v1 = ((m1 - m2) / (m1 + m2)) * u1 + (2 * m2 / (m1 + m2)) * u2;
			v1 = mulVector(v1, b2.friction);
			//v1 = limitVector(v1, velocity_cutoff);
			velocity = v1;
		}
		if (not b2.is_static)
		{
			auto v2 = ((m2 - m1) / (m1 + m2)) * u2 + (2 * m1 / (m1 + m2)) * u1;
			v2 = mulVector(v2, friction);
			//v2 = limitVector(v2, velocity_cutoff);
			b2.velocity = v2;
		}
		//momentum calculations
	}

	void AtRest()
	{	
		at_rest = position.y <= 0 && limitVector(velocity, 0.1).y == 0;
	}

	void update(const float dt)
	{
		position += physics::calculatePosition(velocity, dt);
		setPosition(position);
		//check if bounding box visuals
		glm::mat4 mv_matrix_cube =
			glm::translate(position) *
			glm::scale(scale) *
			glm::mat4(1.0f);
		visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
		visual.proj_matrix = myGraphics.proj_matrix;
	}
};
#endif