#include "particle/particle_TF.h"
ParticleSystem_TF::ParticleSystem_TF(std::string texpath) {
  this->isInitiated = true;
  this->gravity.x = 0, this->gravity.y = 2, this->gravity.z = 0;
  this->friction = 0.0f;

  pos = glm::vec3(-0.710257f, 2.9351f, 10.8066f);

  	// create input layout for particles
  glGenVertexArrays(1, &particlelayout);
  glBindVertexArray(particlelayout);
  {
    glBindVertexBuffers(0, 0, NULL, NULL, NULL);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 0);
    glVertexAttribBinding(2, 0);

    glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 16);
    glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 32);
  }
  glBindVertexArray(0);

  // create buffers & transform feedbacks
  glGenBuffers(1, &emittersbuffer);
  glGenBuffers(3, particlebuffers);
  glGenTransformFeedbacks(3, transformfeedbacks);
  emitters = new Particle_TF[NUM_EMITTERS];
  memset(emitters, 0, NUM_EMITTERS * sizeof(Particle_TF));

  glBindBuffer(GL_ARRAY_BUFFER, emittersbuffer);
  glBufferData(GL_ARRAY_BUFFER, NUM_EMITTERS * sizeof(Particle_TF), NULL, GL_DYNAMIC_DRAW);
  
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

  glBindBuffer(GL_ARRAY_BUFFER, particlebuffers[0]);
  glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle_TF), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, particlebuffers[1]);
  glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle_TF), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, particlebuffers[2]);
  glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle_TF), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0; i < 3; ++i) {
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformfeedbacks[i]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particlebuffers[i]);
  }
  // create random texture

  for (int i = 0; i < 2048 * 4; ++i) data[i] = randomFloatBetween(0.0f, 1.0f);

  glGenTextures(1, &randomtex);
  glBindTexture(GL_TEXTURE_2D, randomtex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1, 0, GL_RGBA, GL_FLOAT, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  mapTexture(parseTexture, texpath.c_str());

  glGenQueries(1, &countquery);
}

void ParticleSystem_TF::update(float time_, float deltaTime,Camera camera) {
  for (int i = 0; i < 2048 * 4; ++i) data[i] = randomFloatBetween(0.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D, randomtex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1, 0, GL_RGBA, GL_FLOAT, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  for (int i = 0; i < NUM_EMITTERS; ++i) {
    this->emitters[i].velocity.w =
        ((this->emitters[i].velocity.w > EMIT_RATE) ? 0.0f : (this->emitters[i].velocity.w + deltaTime * 10));
    this->emitters[i].position = glm::vec4(pos, 1);
    //this->emitters[i].position.y += 0.75f;
  }
  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, this->emittersbuffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_EMITTERS * sizeof(Particle_TF), this->emitters);

  // emit particles
  smokeemitpipeline->Bind();
  glEnable(GL_RASTERIZER_DISCARD);
  glEnable(GL_TEXTURE_2D);
  glBindBuffer(GL_ARRAY_BUFFER, this->particlebuffers[2]);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->transformfeedbacks[2]);
  {
    GLint sampler = glGetUniformLocation(smokeemitpipeline->GetProgram(), "randomTex");
    glProgramUniform1i(smokeemitpipeline->GetProgram(), sampler, 0);

    glProgramUniform1f(smokeemitpipeline->GetProgram(), 1, time_);
    glProgramUniform1f(smokeemitpipeline->GetProgram(), 2, EMIT_RATE);

    glBindVertexArray(this->particlelayout);
    glBindVertexBuffers(0, 1, &(this->emittersbuffer), &offset, &stride);
    GLenum textureUnit = GL_TEXTURE0 + 0;
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, this->randomtex);

    glBeginTransformFeedback(GL_POINTS);
    { glDrawArrays(GL_POINTS, 0, NUM_EMITTERS); }
    glEndTransformFeedback();
  }
  //glFlush();
  //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Particle_data2), Particle_data2);
  // std::cout << Particle_data2[0].velocity.w;
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);

  // update particles
  for (int i = 0; i < 2048 * 4; ++i) data[i] = randomFloatBetween(0.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D, randomtex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 1, 0, GL_RGBA, GL_FLOAT, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  smokeupdatepipeline->Bind();
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->transformfeedbacks[this->currentbuffer]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->particlebuffers[this->currentbuffer]);
  {
    GLint sampler = glGetUniformLocation(smokeemitpipeline->GetProgram(), "randomTex");

    glProgramUniform1f(smokeupdatepipeline->GetProgram(), 0, deltaTime);
    glProgramUniform1f(smokeupdatepipeline->GetProgram(), 1, BUOYANCY);
    glProgramUniform1f(smokeupdatepipeline->GetProgram(), 2, LIFE_SPAN);
    glProgramUniform3fv(smokeupdatepipeline->GetProgram(), 3, 1, glm::value_ptr(pos));
    glProgramUniform1f(smokeemitpipeline->GetProgram(), 4, time_);
    smokeupdatepipeline->Bind();

    glBindVertexArray(this->particlelayout);
    glBindVertexBuffers(0, 1, &(this->particlebuffers[2]), &offset, &stride);
    GLenum textureUnit = GL_TEXTURE0 + 0;
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, this->randomtex);

    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, countquery);
    glBeginTransformFeedback(GL_POINTS);
    {
      // update and add newly generated particles
      glDrawTransformFeedback(GL_POINTS, this->transformfeedbacks[2]);

      // update and add older particles
      if (prevbufferusable) {
        glBindVertexArray(this->particlelayout);
        glBindVertexBuffers(0, 1, &(this->particlebuffers[1 - this->currentbuffer]), &offset, &stride);
        glDrawTransformFeedback(GL_POINTS, this->transformfeedbacks[1 - this->currentbuffer]);
      }
    }
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
  }
  Particle_TF Particle_data[MAX_PARTICLES];
  glFlush();
  glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(Particle_data), Particle_data);
  // std::cout << Particle_data[0].velocity.w;
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
  glDisable(GL_RASTERIZER_DISCARD);
  glGetQueryObjectuiv(countquery, GL_QUERY_RESULT, &count);
  glm::vec3 vdir = camera.Front;
  glm::vec3 eye = camera.Position;
  glUseProgram(0);
  prevbufferusable = true;
}
void ParticleSystem_TF::draw(glm::mat4 projection, glm::mat4 view) {
  glm::vec2 clipradius = glm::vec2(0.0f, 1.0f);
  // render smoke
  billboardpipeline->Bind();
  glProgramUniformMatrix4fv(billboardpipeline->GetProgram(), 0, 1, GL_FALSE, glm::value_ptr(projection));
  glProgramUniformMatrix4fv(billboardpipeline->GetProgram(), 1, 1, GL_FALSE, glm::value_ptr(view));
  glProgramUniform2fv(billboardpipeline->GetProgram(), 2, 1, glm::value_ptr(clipradius));
  glProgramUniform1f(billboardpipeline->GetProgram(), 3, LIFE_SPAN);
  glProgramUniform1i(billboardpipeline->GetProgram(), 4, 0);
  glProgramUniform1i(billboardpipeline->GetProgram(), 5, 1);
  GLint sampler = glGetUniformLocation(billboardpipeline->GetProgram(), "tex0");
  glProgramUniform1i(billboardpipeline->GetProgram(), sampler, 0);

  glBindVertexArray(this->particlelayout);
  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_POINT_SMOOTH);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindVertexBuffers(0, 1, &(this->particlebuffers[this->currentbuffer]), &offset, &stride);
  GLenum textureUnit = GL_TEXTURE0 + 0;
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, parseTexture);
  glDrawTransformFeedback(GL_POINTS, this->transformfeedbacks[this->currentbuffer]);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
  glDisable(GL_BLEND);
  glUseProgram(0);
}
void ParticleSystem_TF::bind_shader(std::string path) {
    smokeemitpipeline = new OpenGLProgramPipeline();
    smokeupdatepipeline = new OpenGLProgramPipeline();
    billboardpipeline = new OpenGLProgramPipeline();
    std::string file_path = "Media/ShadersGL/" + path;
    std::cout << file_path;
    char tmp[256];
    chdir(file_path.c_str());
    auto _ = getcwd(tmp, 256);
    std::cout << _;
    smokeemitpipeline->AddShader(GL_VERTEX_SHADER, "./particle.vert");
    smokeemitpipeline->AddShader(GL_GEOMETRY_SHADER, "./smokeemitter.geom");

    if (!smokeemitpipeline->Assemble()) exit;

    smokeupdatepipeline->AddShader(GL_VERTEX_SHADER, "./smokeupdate.vert");
    smokeupdatepipeline->AddShader(GL_GEOMETRY_SHADER, "./smokeupdate.geom");

    if (!smokeupdatepipeline->Assemble()) exit;

    billboardpipeline->AddShader(GL_VERTEX_SHADER, "./particle.vert");
    billboardpipeline->AddShader(GL_GEOMETRY_SHADER,"./billboard.geom");
    billboardpipeline->AddShader(GL_FRAGMENT_SHADER, "./billboard.frag");
    if (!billboardpipeline->Assemble()) exit;

    chdir("../../../");
    _ = getcwd(tmp, 256);
    std::cout << _;
}