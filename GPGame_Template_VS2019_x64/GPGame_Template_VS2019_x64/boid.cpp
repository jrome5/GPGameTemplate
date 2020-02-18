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

	velocity.x = getRandomFloat(1.0f, -0.5f);
	velocity.y = getRandomFloat(1.0f, -0.5f);
	velocity.z = getRandomFloat(1.0f, -0.5f);

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
		adjustment = limitVector(adjustment, maxSpeed);
		adjustment = subVector(adjustment, velocity);
		adjustment = limitVector(adjustment, maxForce);
	}

	return adjustment;
}


glm::vec3 Boid::separation(vector<Boid> flock) {
	glm::vec3 adjustment = glm::vec3(0.0f, 0.0f, 0.0f);
	
	int total = 0;

	for (int i = 0; i < flock.size(); i++) {

		float dist = distance(flock[i].position, position);

		if (dist != 0 && dist < visionRadius) {
			glm::vec3 diff = subVector(position, flock[i].position);
			diff = divVector(diff, (dist * dist));
			adjustment = addVector(adjustment, diff);

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
		adjustment = subVector(adjustment, position);
		adjustment = limitVector(adjustment, maxSpeed);
		adjustment = subVector(adjustment, velocity);
		adjustment = limitVector(adjustment, maxForce);
	}

	return adjustment;
}

void Boid::behaviour(vector<Boid> flock) {

	// ARE THESE NEEDED?
	glm::vec3 aligned = align(flock);
	glm::vec3 separated = separation(flock);
	glm::vec3 cohesive = cohesion(flock);

	aligned = mulVector(aligned, 1);
	separated = mulVector(aligned, 1);
	cohesive = mulVector(aligned, 0.5);

	acceleration = addVector(acceleration, aligned);
	acceleration = addVector(acceleration, separated);
	acceleration = addVector(acceleration, cohesive);
}

void Boid::update() {
	

	velocity = addVector(velocity, acceleration);
	position = addVector(position, velocity);

	// Will need to set max velocity here

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

void Boid::cage() {
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

/*  THIS WILL BE NEEDED WHERE THEY ARE CREATED

 vector flock = [];

 for (int i = 0; i < 10; i++){

    flock.add(new Boid());

 }

 for (int i = 0; i < flock.size; i++){

	boid.cage();
	boid.behavior();
	boid.update();

 }



*/