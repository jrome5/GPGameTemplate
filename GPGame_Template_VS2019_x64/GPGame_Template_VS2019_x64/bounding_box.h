#pragma once
#include "shapes.h"
#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

namespace bounding_box
{
	class BoundingBox {
	public:
		BoundingBox(const glm::vec3 position, float mass, bool show_box = true);

		~BoundingBox();

		Cube visual;

		void setScale(const float x, const float y, const float z);

		glm::vec3 getPosition() const;

		glm::vec3 getVelocity() const;

		glm::vec3 getScale() const;

		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		glm::vec3 force;
		float mass;
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};
}
#endif //