#pragma once
#include "aabb.h"
#include "shapes.h"
#include "particle.h"
#include "physics.h"

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

class BoundingBox : public Particle {
public:
	BoundingBox(glm::vec3 position, float mass, int identifier) : Particle(position, mass)
	{
		visual.fillColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
		visual.lineColor = glm::vec4(0.2f, 0.2, 0.2, 1.0);
		visual.lineWidth = 5.0f;
		aabb.c = Point{ position.x, position.y, position.z };
		aabb.r = Point{ 1.0f, 1.0f, 1.0f };
		id = identifier;
	}

	~BoundingBox() {};

	Cube visual;
	AABB aabb;
	glm::vec3 scale = {1.0f, 1.0f, 1.0f };
	float friction = 0.98f;
	bool is_static = false;
	bool atRest = false;
	int id;

	void setScale(const float x, const float y, const float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
		aabb.r = Point{ x/2, y/2, z/2 };
	}

	void setPosition(const glm::vec3& pos)
	{
		aabb.c = Point{ pos.x, pos.y, pos.z };
	}

	void checkAtRest()
	{
		if (velocity.y == 0)
			atRest = true;
	}
};
#endif //