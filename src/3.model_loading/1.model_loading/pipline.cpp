#pragma once
#include "pipline.h"

std::string& GetPath(std::string& out, const std::string& str) {
  size_t pos = str.find_last_of("\\/");

  if (pos != std::string::npos) {
#ifdef _WIN32
    out = str.substr(0, pos) + '\\';
#else
    out = str.substr(0, pos) + '/';
#endif
  } else {
    out = "";
  }

  return out;
}
GLuint GLCompileShaderFromFile(GLenum type, const char* file, const char* defines) {
  std::string source;
  char log[1024];
  size_t pos;
  FILE* infile = nullptr;
  GLuint shader = 0;
  GLint length;
  GLint success;
  GLint deflength = 0;

  if (file == nullptr) return 0;

#ifdef _MSC_VER
  fopen_s(&infile, file, "rb");
#elif defined(__APPLE__)
  std::string resfile = GetResource(file);
  infile = fopen(resfile.c_str(), "rb");
#else
  infile = fopen(file, "rb");
#endif

  if (infile == nullptr) return 0;

  fseek(infile, 0, SEEK_END);
  length = ftell(infile);
  fseek(infile, 0, SEEK_SET);

  if (defines != nullptr) deflength = (GLint)strlen(defines);

  source.reserve(length + deflength);
  source.resize(length);

  fread(&source[0], 1, length, infile);

  // add defines
  if (defines != nullptr) {
    pos = source.find("#version");
    pos = source.find('\n', pos) + 1;

    source.insert(pos, defines);
  }

  fclose(infile);

  // process includes (non-recursive)
  pos = source.find("#include");

  while (pos != std::string::npos) {
    size_t start = source.find('\"', pos) + 1;
    size_t end = source.find('\"', start);

    std::string incfile(source.substr(start, end - start));
    std::string path;
    std::string incsource;

    GetPath(path, file);

#ifdef _MSC_VER
    fopen_s(&infile, (path + incfile).c_str(), "rb");
#else
    infile = fopen((path + incfile).c_str(), "rb");
#endif

    if (infile) {
      fseek(infile, 0, SEEK_END);
      length = ftell(infile);
      fseek(infile, 0, SEEK_SET);

      incsource.resize(length);

      fread(&incsource[0], 1, length, infile);
      fclose(infile);

      source.replace(pos, end - pos + 1, incsource);
    }

    pos = source.find("#include", end);
  }

  shader = glCreateShader(type);
  length = (GLint)source.length();

  const GLcharARB* sourcedata = (const GLcharARB*)source.data();

  glShaderSource(shader, 1, &sourcedata, &length);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success != GL_TRUE) {
    glGetShaderInfoLog(shader, 1024, &length, log);
    log[length] = 0;

    std::cout << log << "\n";
    glDeleteShader(shader);

    return 0;
  }

  return shader;
}

OpenGLProgramPipeline::OpenGLProgramPipeline() {
  pipeline = 0;
  program = 0;

  glGenProgramPipelines(1, &pipeline);
  memset(shaders, 0, sizeof(shaders));
}

OpenGLProgramPipeline::~OpenGLProgramPipeline() {
  if (program != 0) glDeleteProgram(program);

  if (pipeline != 0) glDeleteProgramPipelines(1, &pipeline);

  pipeline = 0;
  program = 0;
}

bool OpenGLProgramPipeline::AddShader(GLenum type, const char* file, const char* defines) {
  int index = -1;

  switch (type) {
    case GL_VERTEX_SHADER: index = 0; break;
    case GL_TESS_CONTROL_SHADER: index = 1; break;
    case GL_TESS_EVALUATION_SHADER: index = 2; break;
    case GL_GEOMETRY_SHADER: index = 3; break;
    case GL_FRAGMENT_SHADER: index = 4; break;

    default: return false;
  }

  if (shaders[index] != 0) return false;

  shaders[index] = GLCompileShaderFromFile(type, file, defines);

  if (shaders[index] == 0) return false;

  return true;
}

bool OpenGLProgramPipeline::Assemble() {
  GLbitfield pipelinestages[] = {GL_VERTEX_SHADER_BIT, GL_TESS_CONTROL_SHADER_BIT, GL_TESS_EVALUATION_SHADER_BIT,
                                 GL_GEOMETRY_SHADER_BIT, GL_FRAGMENT_SHADER_BIT};

  if (program != 0) return false;

  char log[1024];
  GLint success = GL_FALSE;
  GLint length = 0;
  GLbitfield stages = 0;

  program = glCreateProgram();

  for (int i = 0; i < 5; ++i) {
    if (shaders[i] != 0) {
      glAttachShader(program, shaders[i]);
      stages |= pipelinestages[i];
    }
  }

  glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (success != GL_TRUE) {
    glGetProgramInfoLog(program, 1024, &length, log);
    log[length] = 0;

    std::cout << log << "\n";
    return false;
  }

  // delete shaders
  for (int i = 0; i < 5; ++i) {
    if (shaders[i] != 0) {
      glDetachShader(program, shaders[i]);
      glDeleteShader(shaders[i]);

      shaders[i] = 0;
    }
  }

  // add to pipeline
  glUseProgramStages(pipeline, stages, program);

  glValidateProgramPipeline(pipeline);
  glGetProgramPipelineiv(pipeline, GL_VALIDATE_STATUS, &success);

  if (success != GL_TRUE) {
    glGetProgramPipelineInfoLog(pipeline, 1024, &length, log);
    log[length] = 0;

    std::cout << log << "\n";
    return false;
  }

  return true;
}

void OpenGLProgramPipeline::Bind() { glBindProgramPipeline(pipeline); }

void OpenGLProgramPipeline::UseProgramStages(OpenGLProgramPipeline* other, GLbitfield stages) {
  if (other->program == 0) return;

  glUseProgramStages(pipeline, stages, other->program);
}