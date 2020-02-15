#include "bounding_box.h"

namespace bounding_box
{
	BoundingBox::BoundingBox(const glm::vec3 position, float mass, bool show_box) : position(position), mass(mass)
	{
		visual.fillColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
		visual.lineColor = glm::vec4(0.2f, 0.2, 0.2, 1.0);
		visual.lineWidth = 5.0f;
	}

	BoundingBox::~BoundingBox()
	{
	}

	void BoundingBox::setScale(const float x, const float y, const float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	glm::vec3 BoundingBox::getPosition() const
	{
		return position;
	}

	glm::vec3 BoundingBox::getVelocity() const
	{
		return velocity;
	}

	glm::vec3 BoundingBox::getScale() const
	{
		return scale;
	}
}