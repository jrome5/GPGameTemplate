#pragma once
#include "AABB.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <list>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Boid {

public:

	Boid(float maxS, float maxF, float vis, float size, int i);
	~Boid();


	// VARIABLES
	glm::vec3  position;
	glm::vec3  velocity;
	//glm::vec3  magnitude; set this later
	glm::vec3  acceleration;
	float     maxSpeed;
	float     maxForce;
	float	  bounds;

	AABB bounding_box;
	int id;

	// METHODS

	/* Seeks to keep boids moving on the same heading as nearby boids
	*/
	glm::vec3 align(vector<Boid> flock);


	/* Stops boids from colliding by maintaining separation from neighbours
	*/
	glm::vec3 separation(vector<Boid> flock);


	/* Steers boids towards average position of nearby boids
	*/
	glm::vec3 cohesion(vector<Boid> flock);

	/* Steers boids towards the master position
	*/
	glm::vec3 follow(vector<Boid> flock, glm::vec3 master);

	/* Implements the three rules, align, separation and cohesion
	*/
	void behaviour(vector<Boid> flock, glm::vec3 master, const float al, const float sep, const float coh, const float fol);


	/* Updates the properties of the boid, position, velocity...etc
	*/
	void update(const float dt);

	void limitVelocity(glm::vec3& v, float limit);

	/* Keeps the boid within the boundaries of a virtual cage
	*/
	void cageJack();

	/* Keeps the boid within the boundaries of a virtual cage
	*/
	void cageShayne();


	/* Calculates the distance between two vectors
	*/
	float distance(glm::vec3 otherBoid, glm::vec3 thisBoid);


	/* Adds two vectors element wise
	*/
	glm::vec3 addVector(glm::vec3 vec1, glm::vec3 vec2);


	/* Subtracts two vectors element wise
	*/
	glm::vec3 subVector(glm::vec3 vec1, glm::vec3 vec2);


	/* Divides a vector by a constant
	*/
	glm::vec3 divVector(glm::vec3 vec, float con);

	/* Multiplies a vector by a constant
	*/
	glm::vec3 mulVector(glm::vec3 vec, float con);

	/* Keeps each element in the vector from exceeding the limit.
	*  If an element exceeds the limit, it will be set to the limit.
	*/
	glm::vec3 limitVector(glm::vec3 vec, float limit);

	/* Normailse a vector
	*/
	glm::vec3 normVector(glm::vec3 vec);
};
