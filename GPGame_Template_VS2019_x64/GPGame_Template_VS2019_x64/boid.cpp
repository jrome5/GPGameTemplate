#include "boid.h"

inline float getRandomFloat(const float upper, const float offset = 0)
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / upper)) + offset;
}

Boid::Boid(float maxS, float maxF, float vis, float size, int i) : bounds(size) {
	// Virtual cage dimensions

	const float vel_magnitude = 0.5f;
	velocity.x = getRandomFloat(vel_magnitude, -vel_magnitude/2);
	velocity.y = getRandomFloat(vel_magnitude, -vel_magnitude/2);
	velocity.z = getRandomFloat(vel_magnitude, -vel_magnitude/2);

	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	position.x = getRandomFloat(bounds);
	position.y = getRandomFloat(bounds);
	position.z = getRandomFloat(bounds);

	maxSpeed = maxS;
	maxForce = maxF;
	id = i;
	bounding_box.c = glm::vec3{ position.x, position.y, position.z };
	bounding_box.r = glm::vec3{ vis, vis, vis };
	// might need to set an ideal height for 3D implementation
};

Boid::~Boid() {
}


glm::vec3 Boid::align(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;
	for (const auto& boid : flock)
	{
		if (boid.id == id)
			continue;
		if (isPointInsideSphere(boid.position, bounding_box))
		{
			adjustment = addVector(adjustment, boid.velocity);
			total++;
		}
	}
	if (total > 0)
	{
		adjustment = divVector(adjustment, total);
	}
	adjustment = normVector(adjustment);
	//adjustment = divVector(subVector(adjustment, velocity), 8);
	return adjustment;
}


glm::vec3 Boid::separation(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;
	for (const auto& boid : flock)
	{
		if (boid.id == id)
		{
			continue;
		}
		if(isPointInsideSphere(boid.position, bounding_box))
		{
			adjustment = subVector(adjustment, (subVector(boid.position, position)));
		}
	}
	return adjustment;
}

glm::vec3 Boid::cohesion(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);

	AABB neighbourhood = bounding_box;
	neighbourhood.r = mulVector(bounding_box.r, 2);
	int total = 0;
	for (int i = 0; i < flock.size(); i++) {
		if (id == flock[i].id)
			continue;
		if (isPointInsideSphere(flock[i].position, neighbourhood)) {
			adjustment = addVector(adjustment, flock[i].position);
			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = subVector(adjustment, position);
		//adjustment = limitVector(adjustment, maxSpeed);
		//adjustment = subVector(adjustment, velocity);
		//adjustment = limitVector(adjustment, maxForce);
	}
	//std::cout << flock[0].velocity.x << "," << flock[0].velocity.y << "," << flock[0].velocity.z << std::endl;
	return adjustment;
}

//glm::vec3 Boid::follow(vector<Boid> flock, glm::vec3 master) {
//	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
//	int total = 0;
//
//	for (int i = 0; i < flock.size(); i++) {
//		if (id == flock[i].id)
//			continue;
//		float dist = distance(flock[i].position, master);
//
//		if (dist != 0 && dist < visionRadius) {
//			adjustment = addVector(adjustment, master);
//			total++;
//		}
//	}
//
//	if (total > 0) {
//		adjustment = divVector(adjustment, total);
//		adjustment = limitVector(adjustment, maxSpeed);
//		adjustment = subVector(adjustment, velocity);
//		adjustment = limitVector(adjustment, maxForce);
//	}
//
//	return adjustment;
//}

void Boid::behaviour(vector<Boid> flock, glm::vec3 master, const float al, const float sep, const float coh, const float fol) {

	glm::vec3 aligned = align(flock);
	glm::vec3 separated = separation(flock);
	glm::vec3 cohesive = cohesion(flock);
//	glm::vec3 following = follow(flock, master);

	aligned = mulVector(aligned, al);
	separated = mulVector(separated, sep);
	cohesive = mulVector(cohesive, coh);
//	following = mulVector(following, fol);

	acceleration = addVector(acceleration, aligned);
	acceleration = addVector(acceleration, separated);
	acceleration = addVector(acceleration, cohesive);
	//acceleration = addVector(acceleration, following);
}

void Boid::update(const float dt) {
	
	//position = addVector(position, velocity);
	velocity = addVector(velocity, acceleration);
	//velocity = limitVector(velocity, maxSpeed);
	limitVelocity(velocity, maxSpeed);
	position = addVector(position, velocity);
	bounding_box.c = glm::vec3{ position.x, position.y, position.z };

	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);  // reset acceleration, is this right??
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

void Boid::cageJack() {
	if (position.x > bounds) {
		velocity.x *= -1;		
	}
	else if (position.x < 0.0f) {
		velocity.x *= -1;		
	}

	if (position.y > bounds) {
		velocity.y *= -1;		
	}
	else if (position.y < 0.0f) {
		velocity.y *= -1;	
	}

	if (position.z > bounds) {
		velocity.z *= -1;	
	}
	else if (position.z < 0.0f) {
		velocity.z *= -1;		
	}
}

void Boid::cageShayne() {
	if (position.x >= bounds) {
		position.x = 0;
	}
	else if (position.x < 0.0f) {
		position.x = bounds;
	}

	if (position.y >= bounds) {
		position.y = 0;
	}
	else if (position.y < 0.0f) {
		position.y = bounds;
	}

	if (position.z >= bounds) {
		position.z = 0;
	}
	else if (position.z < 0.0f) {
		position.z = bounds;
	}
}

