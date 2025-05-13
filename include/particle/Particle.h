#ifndef PARTICLE_H
#define PARTICLE_H
#undef __gl_h_
#include <glad/glad.h>
#include <glm\gtc\matrix_transform.hpp>

class Particle
{
public:
	float lifeSpan;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::mat4 modelMatrix;

	Particle();
	Particle(float _positionX, float _positionY, float _positionZ);
	void update();
	static bool sortParticles(Particle _particleA, Particle _particleB);
	float generateRandomFloat();
};

#endif