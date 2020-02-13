#pragma once
#include "aabb.h"
#include "shapes.h"
#include "particle.h"
#include "physics.h"
#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

class BoundingBox : public Particle {
public:
	BoundingBox(glm::vec3 position, float mass, bool show_box = true) : Particle(position, mass)
	{
		visual.fillColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
		visual.lineColor = glm::vec4(0.2f, 0.2, 0.2, 1.0);
		visual.lineWidth = 5.0f;
	}

	~BoundingBox() {};

	Cube visual;
	glm::vec3 scale = {1.0f, 1.0f, 1.0f };

	void setScale(const float x, const float y, const float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}
};
#endif //