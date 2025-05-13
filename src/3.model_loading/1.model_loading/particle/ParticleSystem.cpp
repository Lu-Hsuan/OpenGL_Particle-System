#include "particle/Helper.h"
#include "particle/ParticleSystem.h"
#include <algorithm>
#include <GLFW/glfw3.h>
#include <iostream>

ParticleSystem::ParticleSystem(){
	this->isInitiated = true;
	this->setParticles();

	//
	this->gravity.x = 0, this->gravity.y = 2, this->gravity.z = 0;
	this->friction = 0.0f;

	double thisTime = glfwGetTime()/10;
	deltaTime = (float)(thisTime - lastTime);
	lastTime = thisTime;
	pos = glm::vec3(-0.710257f, 2.9351f, 10.8066f);
}
void ParticleSystem::setParticles(){
	mapTexture(this->parseTexture, "resources/textures/fire.jpg");
    const int vertex_n = 160;
    glm::vec2 texture[vertex_n][4];
    glm::vec3 position[vertex_n][4];
    glm::vec4 colour[vertex_n][4];
	float high = 0.1f, low = -0.1f;
    for (int i = 0; i < vertex_n; i++) {
		float t1 = randomFloatBetween(0.f, 0.225f);
		float t2 = randomFloatBetween(0.f, 0.225f);
		float t3 = randomFloatBetween(0.f, 0.225f);
		texture[i][0] = glm::vec2(0.f, t1);
		texture[i][1] = glm::vec2(0.f, 0.f);
		texture[i][2] = glm::vec2(t2, t2);
		texture[i][3] = glm::vec2(t3, 0.f);
		for (int j = 0; j < 4; j++) {
			float r1 = randomFloatBetween(low, high);
			float r2 = randomFloatBetween(low, high);
            float r3 = randomFloatBetween(-40.f, -50.f);
			float r4 = randomFloatBetween(-0.1f, 0.1f);
			position[i][j] = glm::vec3(r1,r2, r4);
			colour[i][j] = glm::vec4(1.0, 1.0, 0.0, isInBetween(r3));
		}
	}

	GLuint posVBO;
	GLuint texVBO;
	GLuint colourVBO;

	glGenVertexArrays(1, &particleVAO_ID);
    glBindVertexArray(this->particleVAO_ID);

    glEnableVertexAttribArray(0);
	glGenBuffers(1, &posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, /*3000*/ 4 * 80 * sizeof(glm::vec3), position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &colourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colourVBO);
	glBufferData(GL_ARRAY_BUFFER, /*3000*/  4 * 80 * sizeof(glm::vec4), colour, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(2);
	glGenBuffers(1, &texVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texVBO);
	glBufferData(GL_ARRAY_BUFFER, /*3000*/ 4 * 80 * sizeof(glm::vec2), texture, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindVertexArray(0);
}


void ParticleSystem::update(){
	double thisTime = glfwGetTime();
    deltaTime = (float)(thisTime - lastTime)/10;
	lastTime = thisTime;

	for (std::vector<Particle>::iterator i = this->particles.begin(); i != this->particles.end();) {
		i->velocity = computeVelocity(i->velocity, i->acceleration + gravity, deltaTime);
		i->position = computeEuler(i->position, i->velocity, i->acceleration + gravity, deltaTime);

		glm::vec3 diff = i->position;
		i->acceleration.x -= diff.x * (0.03f - i->lifeSpan*0.03);
		i->acceleration.z -= diff.z * (0.03f - i->lifeSpan*0.03);
		i->lifeSpan -= deltaTime;
		if (i->lifeSpan <= 0.0f)
			i = this->particles.erase(i);
		else ++i;
	}

	for (std::vector<Particle>::iterator i = particles.begin(); i != particles.end(); ++i)
		i->update();

	for (int i = 0; i < deltaTime * 1000; ++i) this->particles.push_back(Particle());
	std::sort(this->particles.begin(), this->particles.end(), Particle::sortParticles);
}