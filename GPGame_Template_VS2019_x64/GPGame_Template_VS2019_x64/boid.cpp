#include "boid.h"

inline float getRandomFloat(const float upper, const float offset = 0)
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / upper)) + offset;
}

Boid::Boid(float maxS, float maxF, float vis) {
	// Virtual cage dimensions
	width = 20;
	length = 20;
	height = 20;

	position.x = getRandomFloat(width);
	position.y = getRandomFloat(height);
	position.z = getRandomFloat(length);

	velocity.x = getRandomFloat(0.1f, -0.05f);
	velocity.y = getRandomFloat(0.1f, -0.05f);
	velocity.z = getRandomFloat(0.1f, -0.05f);

	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	maxSpeed = maxS;
	maxForce = maxF;
	visionRadius = vis;
	// might need to set an ideal height for 3D implementation
};

Boid::~Boid() {

}


glm::vec3 Boid::align(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;

	for (int i = 0; i < flock.size(); i++)
	{
		float dist = distance(flock[i].position, position);

		if (dist != 0 && dist < visionRadius) {
			adjustment = addVector(adjustment, flock[i].velocity);
			total++;
		}	
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = normVector(adjustment);
		adjustment = limitVector(adjustment, maxSpeed);
		adjustment = subVector(adjustment, velocity);
		adjustment = limitVector(adjustment, maxForce);
	}
	//std::cout << flock[0].velocity.x << "," << flock[0].velocity.y << "," << flock[0].velocity.z << std::endl;

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
			//diff = divVector(diff, dist);
			adjustment = addVector(adjustment, diff);

			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = normVector(adjustment);
		adjustment = limitVector(adjustment, maxSpeed);
		adjustment = subVector(adjustment, velocity);
		adjustment = limitVector(adjustment, maxForce);
	}
	//std::cout << flock[0].velocity.x << "," << flock[0].velocity.y << "," << flock[0].velocity.z << std::endl;
	return adjustment;
}

glm::vec3 Boid::cohesion(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;

	for (int i = 0; i < flock.size(); i++) {
		
		float dist = distance(flock[i].position, position);

		if (dist != 0 && dist < visionRadius) {
			adjustment = addVector(adjustment, flock[i].position);
			total++;		
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		//adjustment = subVector(adjustment, position);
		//adjustment = limitVector(adjustment, maxSpeed);
		//adjustment = subVector(adjustment, velocity);
		adjustment = limitVector(adjustment, maxForce);
	}
	//std::cout << flock[0].velocity.x << "," << flock[0].velocity.y << "," << flock[0].velocity.z << std::endl;
	return adjustment;
}

glm::vec3 Boid::follow(vector<Boid> flock, glm::vec3 master) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	int total = 0;

	for (int i = 0; i < flock.size(); i++) {

		float dist = distance(flock[i].position, master);

		if (dist != 0 && dist < visionRadius) {
			adjustment = addVector(adjustment, master);
			total++;
		}
	}

	if (total > 0) {
		adjustment = divVector(adjustment, total);
		adjustment = limitVector(adjustment, maxSpeed);
		adjustment = subVector(adjustment, velocity);
		adjustment = limitVector(adjustment, maxForce);
	}

	return adjustment;
}

void Boid::behaviour(vector<Boid> flock, glm::vec3 master, const float al, const float sep, const float coh, const float fol) {

	glm::vec3 aligned = align(flock);
	glm::vec3 separated = separation(flock);
	glm::vec3 cohesive = cohesion(flock);
	glm::vec3 following = follow(flock, master);

	aligned = mulVector(aligned, al);
	separated = mulVector(aligned, sep);
	cohesive = mulVector(aligned, coh);
	following = mulVector(following, fol);

	acceleration = addVector(acceleration, aligned);
	acceleration = addVector(acceleration, separated);
	acceleration = addVector(acceleration, cohesive);
	//acceleration = addVector(acceleration, following);

}

void Boid::update(const float dt) {
	
	//position = addVector(position, velocity);
	velocity = addVector(velocity, acceleration);
	velocity = limitVector(velocity, maxSpeed);
	position = addVector(position, velocity);

	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);  // reset acceleration, is this right??
}

float Boid::distance(glm::vec3 otherBoid, glm::vec3 thisBoid) {
	float dist = sqrt(pow((otherBoid.x - thisBoid.x), 2) + 
		              pow((otherBoid.y - thisBoid.y), 2) + 
		              pow((otherBoid.z - thisBoid.z), 2)
	                 );
	return dist;
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
	else if (limited.x < -limit) {
		limited.x = -limit;
	}

	if (limited.y > limit) {
		limited.y = limit;
	}
	else if (limited.y < -limit) {
		limited.y = -limit;
	}

	if (limited.z > limit) {
		limited.z = limit;
	}
	else if (limited.z < -limit) {
		limited.z = -limit;
	}

	return limited;
}

void Boid::cageJack() {
	if (position.x > width) {
		velocity.x *= -1;		
	}
	else if (position.x < 0.0f) {
		velocity.x *= -1;		
	}

	if (position.y > length) {
		velocity.y *= -1;		
	}
	else if (position.y < 0.0f) {
		velocity.y *= -1;	
	}

	if (position.z > height) {
		velocity.z *= -1;	
	}
	else if (position.z < 0.0f) {
		velocity.z *= -1;		
	}
}

void Boid::cageShayne() {
	if (position.x > width) {
		position.x = 0;
	}
	else if (position.x < 0.0f) {
		position.x = width;
	}

	if (position.y > length) {
		position.y = 0;
	}
	else if (position.y < 0.0f) {
		position.y = length;
	}

	if (position.z > height) {
		position.z = 0;
	}
	else if (position.z < 0.0f) {
		position.z = height;
	}
}

