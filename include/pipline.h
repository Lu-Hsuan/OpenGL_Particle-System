#pragma once
#undef __gl_h_
#include <glad/glad.h>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include<string>
GLuint GLCompileShaderFromFile(GLenum type, const char* file, const char* defines);
class OpenGLProgramPipeline {
 private:
  GLuint shaders[5];
  GLuint pipeline;
  GLuint program;

 public:
  OpenGLProgramPipeline();
  ~OpenGLProgramPipeline();

  bool AddShader(GLenum type, const char* file, const char* defines = nullptr);
  bool Assemble();

  void Bind();
  void UseProgramStages(OpenGLProgramPipeline* other, GLbitfield stages);

  inline GLuint GetProgram() const { return program; }
};

