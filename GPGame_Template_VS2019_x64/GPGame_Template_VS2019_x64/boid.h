#pragma once

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

	Boid(double maxS, double maxF, double vis);
	~Boid();

protected:

	// VARIABLES

	glm::vec3  position;
	glm::vec3  velocity;
	//glm::vec3  magnitude; set this later
	glm::vec3  acceleration;
	float     maxSpeed;
	float     maxForce;
	float     visionRadius;

	float width;
	float length;
	float height;


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


	/* Implements the three rules, align, separation and cohesion
	*/
	void behaviour(vector<Boid> flock);


	/* Updates the properties of the boid, position, velocity...etc
	*/
	void update();


	/* Keeps the boid within the boundaries of a virtual cage
	*/
	void cage();


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


	/* Keeps each element in the vector from exceeding the limit.
	*  If an element exceeds the limit, it will be set to the limit.
	*/
	glm::vec3 limitVector(glm::vec3 vec, float limit);
};

