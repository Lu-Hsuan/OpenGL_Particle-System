#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#undef __gl_h_
#include <glad/glad.h>
#include "Particle.h"
#include <vector>

class ParticleSystem
{
public:
	GLuint particleVAO_ID;
	GLuint parseTexture;
	GLuint playerTex;
	GLuint poinTex;
	glm::vec3 pos;
	// Particle System properties
	glm::vec3 gravity;
	double lastTime;
	float deltaTime;
	float friction;
	float invCount;

	std::vector<Particle> particles;
	bool isInitiated;

	ParticleSystem();
	void setParticles();
	void update();
    void draw();
};

#endif