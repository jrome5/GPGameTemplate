#include "boid.h"

inline float getRandomFloat(const float upper, const float offset = 0)
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / upper)) + offset;
}

Boid::Boid(float maxS, float vis, glm::vec3 cage_position, glm::vec3 size, int i) {
	// Virtual cage dimensions
	const float vel_magnitude = 0.5f;
	velocity.x = getRandomFloat(vel_magnitude, -vel_magnitude / 2);
	velocity.y = getRandomFloat(vel_magnitude, -vel_magnitude / 2);
	velocity.z = getRandomFloat(vel_magnitude, -vel_magnitude / 2);

	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	bounds.x = cage_position.x + size.x/2;
	bounds.y = cage_position.y + size.y/2;
	bounds.z = cage_position.z + size.z/2;
	position.x = 2.0f + getRandomFloat(bounds.x);
	position.y = getRandomFloat(bounds.y);
	position.z = 2.0f + getRandomFloat(bounds.z);
	bounding_box.c = position;
	neighbourhood.c = position;
	bounding_box.r = glm::vec3{ vis, vis, vis };
	neighbourhood.r = mulVector(bounding_box.r, 8);
	maxSpeed = maxS;

	visionRadius = 0.25;
	id = i;
	// might need to set an ideal height for 3D implementation
};

Boid::~Boid() {
}


glm::vec3 Boid::align(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;

	for (int i = 0; i < flock.size(); i++)
	{
		if (isPointInsideSphere(flock[i].position, bounding_box)){
			adjustment = addVector(adjustment, flock[i].velocity);
			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = normVector(adjustment);
		adjustment = subVector(adjustment, velocity);
	}


	return adjustment;
}

glm::vec3 Boid::separation(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);

	int total = 0;

	for (int i = 0; i < flock.size(); i++) {

		float dist = distance(flock[i].position, position);

		if (dist > 0 && dist < visionRadius) {
			glm::vec3 diff = subVector(position, flock[i].position);
			diff = normVector(diff);
			diff = divVector(diff, (dist * dist));
			adjustment = addVector(adjustment, diff);

			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = normVector(adjustment);
		adjustment = subVector(adjustment, velocity);

	}
	return adjustment;
}

glm::vec3 Boid::cohesion(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;

	for (int i = 0; i < flock.size(); i++) {

		if (isPointInsideSphere(flock[i].position, bounding_box)) {
			adjustment = addVector(adjustment, flock[i].position);
			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = subVector(adjustment, position);
		adjustment = subVector(adjustment, velocity);
	}

	return adjustment;
}

glm::vec3 Boid::falcon(vector<Boid> flock, glm::vec3 master) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;

	for (int i = 0; i < flock.size(); i++) {
		if (id == flock[i].id)
			continue;
	if (isPointInsideSphere(master, neighbourhood)) {
			adjustment = addVector(adjustment, master);
			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = subVector(adjustment, position);
		adjustment = subVector(adjustment, velocity);
	}

	return -adjustment;
}

void Boid::behaviour(vector<Boid> flock, glm::vec3 master, const float al, const float sep, const float coh, const float fal) {
	
	glm::vec3 aligned = align(flock);
	glm::vec3 separated = separation(flock);
	glm::vec3 cohesive = cohesion(flock);
	glm::vec3 hunting = falcon(flock, master);

	aligned = mulVector(aligned, al);
	separated = mulVector(separated, sep);
	cohesive = mulVector(cohesive, coh);
	hunting = mulVector(hunting, fal);

	acceleration = addVector(acceleration, aligned);
	acceleration = addVector(acceleration, separated);
	acceleration = addVector(acceleration, cohesive);
	acceleration = addVector(acceleration, hunting);
}

void Boid::update(const float dt) {

	velocity = addVector(velocity, acceleration);
	limitVelocity(velocity, maxSpeed);
	position = addVector(position, velocity);
	bounding_box.c = glm::vec3{ position.x, position.y, position.z };
	neighbourhood.c = bounding_box.c;
	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);  
}

float Boid::distance(glm::vec3 otherBoid, glm::vec3 thisBoid) {
	float dist = (pow((otherBoid.x - thisBoid.x), 2) +
		pow((otherBoid.y - thisBoid.y), 2) +
		pow((otherBoid.z - thisBoid.z), 2)
		);
	return dist;
}

void Boid::limitVelocity(glm::vec3& v, float limit)
{
	const float mag = abs(v.x) + abs(v.y) + abs(v.z);
	if (mag > limit)
	{
		v.x = (v.x / mag) * limit;
		v.y = (v.y / mag) * limit;
		v.z = (v.z / mag) * limit;
	}
}

glm::vec3 Boid::addVector(glm::vec3 vec1, glm::vec3 vec2) {
	glm::vec3 result = glm::vec3((vec1.x + vec2.x),
		(vec1.y + vec2.y),
		(vec1.z + vec2.z)
	);
	return result;
}

glm::vec3 Boid::subVector(glm::vec3 vec1, glm::vec3 vec2) {
	glm::vec3 result = glm::vec3((vec1.x - vec2.x),
		(vec1.y - vec2.y),
		(vec1.z - vec2.z)
	);
	return result;
}

glm::vec3 Boid::divVector(glm::vec3 vec, float con) {
	glm::vec3 result = glm::vec3((vec.x / con),
		(vec.y / con),
		(vec.z / con)
	);
	return result;
}

glm::vec3 Boid::mulVector(glm::vec3 vec, float con) {
	glm::vec3 result = glm::vec3((vec.x * con),
		(vec.y * con),
		(vec.z * con)
	);
	return result;
}

glm::vec3 Boid::normVector(glm::vec3 vec) {

	glm::vec3 temp = vec;
	if (vec.x == 0 and vec.y == 0 and vec.z == 0)
		return vec;

	float mag = sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));

	if (vec.x != 0) {

		temp.x = vec.x / mag;
	}

	if (vec.y != 0) {

		temp.y = vec.y / mag;
	}

	if (vec.z != 0) {

		temp.z = vec.z / mag;
	}

	return temp;
}

glm::vec3 Boid::limitVector(glm::vec3 vec, float limit) {

	glm::vec3 limited = vec;

	if (limited.x > limit) {
		limited.x = limit;
	}
	else if (limited.x < 0) {
		limited.x = -limit;
	}

	if (limited.y > limit) {
		limited.y = limit;
	}
	else if (limited.y < 0) {
		limited.y = -limit;
	}

	if (limited.z > limit) {
		limited.z = limit;
	}
	else if (limited.z < 0) {
		limited.z = -limit;
	}

	return limited;
}

void Boid::cage() {
	if (position.x >= bounds.x) {
		position.x = 0;
	}
	else if (position.x < 0.0f) {
		position.x = bounds.x;
	}

	if (position.y >= bounds.y) {
		position.y = 0;
	}
	else if (position.y < 0.0f) {
		position.y = bounds.y;
	}

	if (position.z >= bounds.z) {
		position.z = 0;
	}
	else if (position.z < 0.0f) {
		position.z = bounds.z;
	}
}

