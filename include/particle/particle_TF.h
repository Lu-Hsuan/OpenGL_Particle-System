#pragma once
#undef __gl_h_
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <learnopengl/camera.h>
#include "particle/Helper.h"
#include "bitonicsort.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#pragma warning(disable : 4996)
#define NUM_EMITTERS 1
#define MAX_PARTICLES 8192
#define THREADGROUP_SIZE 256
//
//#define EMIT_RATE 0.02f
//#define LIFE_SPAN 3.0f
//#define BUOYANCY 0.002f

#define BUDDHA_SCALE 25.0f
#define BOWL_SCALE 0.005f

#define BOWL1_POSITION -0.710257f, 2.2351f, 10.8066f
#define BOWL2_POSITION 2.5f, 0.0f, -0.5f

struct Particle_TF {
  glm::vec4 position;
  glm::vec4 velocity;
  glm::vec4 color;
};

class ParticleSystem_TF {
 public:
  GLuint particleVAO_ID;
  GLuint parseTexture;
  GLuint playerTex;
  GLuint poinTex;
  glm::vec3 pos;
  // Particle System properties
  float EMIT_RATE = 0.02f;
  float LIFE_SPAN = 3.0f;
  float BUOYANCY = 0.002f;
  glm::vec3 gravity;
  double lastTime;
  float friction;
  float invCount;
  int number_emt;

  bool m_isFirst;
  unsigned int m_currVB;
  unsigned int m_currTFB;
  int currentbuffer = 0;
  GLuint particlebuffers[3];
  GLuint transformfeedbacks[3];
  GLuint randomtex;
  Particle_TF* emitters;
  bool isInitiated;
  GLuint particlelayout = 0;
  GLuint countquery = 0;
  GLuint emittersbuffer = 0;

  OpenGLProgramPipeline* smokeemitpipeline = nullptr;
  OpenGLProgramPipeline* smokeupdatepipeline = nullptr;
  OpenGLProgramPipeline* billboardpipeline = nullptr;
  bool prevbufferusable = false;
  GLintptr offset = 0;
  GLsizei stride = sizeof(Particle_TF);
  GLuint count = 0;
  float data[2048 * 4];
  ParticleSystem_TF(std::string );
  //void setParticles();
  void update(float, float,Camera);
  void draw(glm::mat4 , glm::mat4);
  void bind_shader(std::string);
};