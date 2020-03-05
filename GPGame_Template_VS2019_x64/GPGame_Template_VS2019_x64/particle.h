#pragma once
#include <vector>
#include "shapes.h"
#include "physics.h"
#include <glm/glm.hpp>
#include <cmath>

constexpr float pi = 3.14159265359;

class Particle
{
public:
	Particle(const glm::vec3 p, const int index, const int number_of_particles, const glm::vec4 fill)
	{
		position = p;
		emitter_position = p;
		lifetime = physics::getRandomFloat(4.0);
		velocity = glm::vec3(0.0f, 3.0f, 0.0f);
		decay = 1.0f;
		colour = fill;
		phi = std::acos(1.0f - (2.0f * index + 0.5f) / number_of_particles);
		theta = pi * (1.0f + sqrt(5.0f)) * index;
		visual.Load();
		visual.fillColor = fill;
		visual.lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	};

	float mass = 1.0f;
	float lifetime = 0.0f;
	float age = 0.0f;
	float decay;
	float phi;
	float theta;

	glm::vec3 emitter_position;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 scale = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec4 colour;
	Square visual;

	bool checkExpired(const float dt)
	{
	}

	void resetPosition()   // NEED TO CALL THIS SOMEWHERE TO FIRE MORE FIREWORKS
	{
		position = emitter_position;
		velocity = glm::vec3(0.0f, 3.0f, 0.0f);
		decay = 1.0f;
		age = 0.0f;
		visual.fillColor = colour;
	}

	void update(const float dt)
	{
		age += dt / 2.0;

		float x = std::cos(theta) * std::sin(phi);
		float z = std::cos(theta) * std::cos(phi);
		float y = std::sin(theta);

		glm::vec3 accel;

		accel.x = x;
		accel.y = y - 1.5;
		accel.z = z;

		velocity += physics::calculateVelocity(accel, dt);
		position += physics::calculatePosition(velocity, dt);
		decay = (lifetime - age) / lifetime;
	}

	void updateVisual()
	{
		visual.fillColor.r = std::max(0.0f, visual.fillColor.r * decay);
		visual.fillColor.g = std::max(0.0f, visual.fillColor.g * decay);
		visual.fillColor.b = std::max(0.0f, visual.fillColor.b * decay);
		visual.fillColor.a = std::max(0.0f, visual.fillColor.a * decay);

	//	visual.fillColor = glm::vec4(0.0f, decay, 0, decay);         // Decay transparency

		glm::mat4 mv_matrix_tri =
			glm::translate(position) *
			glm::scale(glm::vec3(0.01f, 0.01f, 0.01f)) * //TODO REMOVE MAYBE
			glm::mat4(1.0f);
		visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_tri;

		visual.mv_matrix[0][0] = scale.x;
		visual.mv_matrix[0][1] = 0.0f;
		visual.mv_matrix[0][2] = 0.0f;

		visual.mv_matrix[1][0] = 0.0f;
		visual.mv_matrix[1][1] = -scale.y; //flip mesh
		visual.mv_matrix[1][2] = 0.0f;

		visual.mv_matrix[2][0] = 0.0f;
		visual.mv_matrix[2][1] = 0.0f;
		visual.mv_matrix[2][2] = scale.z;

		visual.proj_matrix = myGraphics.proj_matrix;
	}

};

class Emitter
{
public:
	Emitter(const glm::vec3 pos, const int count) : position(pos), number_of_particles(count)
	{
	};

	~Emitter() {};

	void spawn(const glm::vec4 fill)
	{
		for (int i = 0; i < number_of_particles; i++)
		{
			Particle p(position, i, number_of_particles, fill);
			particles.push_back(p);
		}
	}

	Particle& getParticle(const int i)
	{
		return particles[i];
	}

	glm::vec3 getPosition()
	{
		return position;
	}

	bool checkExpired(const float dt)
	{
		age += dt;
		if (age >= lifetime)
		{
			age = 0.0f;
			return true;
		}
		return false;
	}


	void update(const float dt)
	{
		visual.mv_matrix = myGraphics.viewMatrix *
			glm::translate(glm::vec3(position)) *
			glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)) *
			glm::mat4(1.0f);
		visual.proj_matrix = myGraphics.proj_matrix;
	}

	float age = 0.0f;
	float lifetime = 6.0f;
	int number_of_particles = 0;
	std::vector<Particle> particles;
	Cube visual;
	glm::vec3 position;
};