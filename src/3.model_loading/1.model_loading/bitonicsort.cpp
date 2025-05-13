#pragma once
#include "bitonicsort.h"
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

template<typename value_type, typename compare> OrderedArray<value_type, compare>::OrderedArray() {
  data = nullptr;
  size = 0;
  capacity = 0;
}

template <typename value_type, typename compare>
OrderedArray<value_type, compare>::OrderedArray(const OrderedArray& other) {
  data = nullptr;
  size = 0;
  capacity = 0;

  this->operator=(other);
}

template <typename value_type, typename compare>
OrderedArray<value_type, compare>::~OrderedArray() {
  Destroy();
}

template <typename value_type, typename compare>
size_t OrderedArray<value_type, compare>::_find(const value_type& value) const {
  size_t low = 0;
  size_t high = size;
  size_t mid = (low + high) / 2;

  while (low < high) {
    if (comp(data[mid], value))
      low = mid + 1;
    else if (comp(value, data[mid]))
      high = mid;
    else
      return mid;

    mid = (low + high) / 2;
  }

  return low;
}

template <typename value_type, typename compare>
typename OrderedArray<value_type, compare>::pairib OrderedArray<value_type, compare>::Insert(const value_type& value) {
  size_t i = 0;

  Reserve(size + 1);

  if (size > 0) {
    i = _find(value);

    if (i < size && !(comp(data[i], value) || comp(value, data[i]))) return pairib(SIZE_MAX, false);

    size_t count = (size - i);
    new (data + size) value_type;

    for (size_t j = count; j > 0; --j) data[i + j] = data[i + j - 1];
  } else {
    new (data) value_type;
  }

  data[i] = value;
  ++size;

  return pairib(i, true);
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::Erase(const value_type& value) {
  size_t i = Find(value);

  if (i != npos) {
    size_t count = (size - i) - 1;

    for (size_t j = 0; j < count; ++j) data[i + j] = data[i + j + 1];

    (data + i + count)->~value_type();
    --size;

    if (size == 0) Clear();
  }
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::EraseAt(size_t index) {
  if (index < size) {
    size_t count = (size - index) - 1;

    for (size_t j = 0; j < count; ++j) data[index + j] = data[index + j + 1];

    (data + index + count)->~value_type();
    --size;

    if (size == 0) Clear();
  }
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::Reserve(size_t newcap) {
  if (capacity < newcap) {
    size_t diff = newcap - capacity;
    diff = std::max<size_t>(diff, 10);

    value_type* newdata = (value_type*)malloc((capacity + diff) * sizeof(value_type));

    for (size_t i = 0; i < size; ++i) new (newdata + i) value_type(data[i]);

    for (size_t i = 0; i < size; ++i) (data + i)->~value_type();

    if (data) free(data);

    data = newdata;
    capacity = capacity + diff;
  }
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::Clear() {
  for (size_t i = 0; i < size; ++i) (data + i)->~value_type();

  size = 0;
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::Destroy() {
  if (data) {
    for (size_t i = 0; i < size; ++i) (data + i)->~value_type();

    free(data);
  }

  data = nullptr;
  size = 0;
  capacity = 0;
}

template <typename value_type, typename compare>
size_t OrderedArray<value_type, compare>::Find(const value_type& value) const {
  if (size > 0) {
    size_t ind = _find(value);

    if (ind < size) {
      if (!(comp(data[ind], value) || comp(value, data[ind]))) return ind;
    }
  }

  return npos;
}

template <typename value_type, typename compare>
size_t OrderedArray<value_type, compare>::LowerBound(const value_type& value) const {
  // NOTE: returns the index of the first element which is not greater
  if (size > 0) {
    size_t ind = _find(value);

    if (!(comp(data[ind], value) || comp(value, data[ind])))
      return ind;
    else if (ind > 0)
      return ind - 1;
  }

  return npos;
}

template <typename value_type, typename compare>
size_t OrderedArray<value_type, compare>::UpperBound(const value_type& value) const {
  // NOTE: returns the index of the first element which is not smaller
  return _find(value);
}

template <typename value_type, typename compare>
OrderedArray<value_type, compare>& OrderedArray<value_type, compare>::operator=(const OrderedArray& other) {
  if (&other == this) return *this;

  Clear();
  Reserve(other.capacity);

  size = other.size;

  for (size_t i = 0; i < size; ++i) new (data + i) value_type(other.data[i]);

  return *this;
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::FastCopy(const OrderedArray& other) {
  if (&other != this) {
    Reserve(other.capacity);

    size = other.size;
    memcpy(data, other.data, size * sizeof(value_type));
  }
}

template <typename value_type, typename compare>
void OrderedArray<value_type, compare>::Swap(OrderedArray& other) {
  if (&other == this) return;

  std::swap(capacity, other.capacity);
  std::swap(size, other.size);
  std::swap(data, other.data);
}

template <typename value_type, typename compare>
std::ostream& operator<<(std::ostream& os, const OrderedArray<value_type, compare>& arr) {
  for (size_t i = 0; i < arr.size(); ++i) os << arr[i] << " ";

  return os;
}



bool GLCreateEffectFromFile(const char* vsfile,
                            const char* tcsfile,
                            const char* tesfile,
                            const char* gsfile,
                            const char* psfile,
                            OpenGLEffect** effect,
                            const char* defines) {
  char log[1024];
  OpenGLEffect* neweffect;
  GLuint vertexshader = 0;
  GLuint tesscontrolshader = 0;
  GLuint tessshader = 0;
  GLuint tessevalshader = 0;
  GLuint geometryshader = 0;
  GLuint fragmentshader = 0;
  GLint success = GL_FALSE;
  GLint length = 0;

  // these are mandatory
  if (0 == (vertexshader = GLCompileShaderFromFile(GL_VERTEX_SHADER, vsfile, defines))) return false;

  if (0 == (fragmentshader = GLCompileShaderFromFile(GL_FRAGMENT_SHADER, psfile, defines))) {
    glDeleteShader(vertexshader);
    return false;
  }

  // others are optional
  if (tesfile != nullptr) {
    if (0 == (tessevalshader = GLCompileShaderFromFile(GL_TESS_EVALUATION_SHADER, tesfile, defines))) {
      glDeleteShader(vertexshader);
      glDeleteShader(fragmentshader);

      return false;
    }
  }

  if (tcsfile != nullptr) {
    if (0 == (tesscontrolshader = GLCompileShaderFromFile(GL_TESS_CONTROL_SHADER, tcsfile, defines))) {
      glDeleteShader(vertexshader);
      glDeleteShader(fragmentshader);

      if (tessevalshader != 0) glDeleteShader(tessevalshader);

      return false;
    }
  }

  if (gsfile != nullptr) {
    if (0 == (geometryshader = GLCompileShaderFromFile(GL_GEOMETRY_SHADER, gsfile, defines))) {
      glDeleteShader(vertexshader);
      glDeleteShader(fragmentshader);

      if (tessevalshader != 0) glDeleteShader(tessevalshader);

      if (tesscontrolshader != 0) glDeleteShader(tesscontrolshader);

      return false;
    }
  }

  neweffect = new OpenGLEffect();
  neweffect->program = glCreateProgram();

  glAttachShader(neweffect->program, vertexshader);
  glAttachShader(neweffect->program, fragmentshader);

  if (tessevalshader != 0) glAttachShader(neweffect->program, tessevalshader);

  if (tesscontrolshader != 0) glAttachShader(neweffect->program, tesscontrolshader);

  if (geometryshader != 0) glAttachShader(neweffect->program, geometryshader);

  glLinkProgram(neweffect->program);
  glGetProgramiv(neweffect->program, GL_LINK_STATUS, &success);

  if (success != GL_TRUE) {
    glGetProgramInfoLog(neweffect->program, 1024, &length, log);
    log[length] = 0;

    std::cout << log << "\n";

    glDeleteProgram(neweffect->program);
    delete neweffect;

    return false;
  }

  neweffect->BindAttributes();
  neweffect->QueryUniforms();

  // delete shaders
  glDetachShader(neweffect->program, vertexshader);
  glDeleteShader(vertexshader);

  glDetachShader(neweffect->program, fragmentshader);
  glDeleteShader(fragmentshader);

  if (tessevalshader != 0) {
    glDetachShader(neweffect->program, tessevalshader);
    glDeleteShader(tessevalshader);
  }

  if (tesscontrolshader != 0) {
    glDetachShader(neweffect->program, tesscontrolshader);
    glDeleteShader(tesscontrolshader);
  }

  if (geometryshader != 0) {
    glDetachShader(neweffect->program, geometryshader);
    glDeleteShader(geometryshader);
  }

  (*effect) = neweffect;
  return true;
}
bool GLCreateComputeProgramFromFile(const char* csfile, OpenGLEffect** effect, const char* defines) {
  char log[1024];
  OpenGLEffect* neweffect;
  GLuint shader = 0;
  GLint success;
  GLint length;

  if (0 == (shader = GLCompileShaderFromFile(GL_COMPUTE_SHADER, csfile, defines))) return false;

  neweffect = new OpenGLEffect();
  neweffect->program = glCreateProgram();

  glAttachShader(neweffect->program, shader);
  glLinkProgram(neweffect->program);
  glGetProgramiv(neweffect->program, GL_LINK_STATUS, &success);

  if (success != GL_TRUE) {
    glGetProgramInfoLog(neweffect->program, 1024, &length, log);
    log[length] = 0;

    std::cout << log << "\n";

    glDeleteProgram(neweffect->program);
    delete neweffect;

    return false;
  }

  neweffect->QueryUniforms();

  glDetachShader(neweffect->program, shader);
  glDeleteShader(shader);

  (*effect) = neweffect;
  return true;
}

uint32_t Log2OfPow2(uint32_t x) {
  uint32_t ret = 0;

  while (x >>= 1) ++ret;

  return ret;
}

uint32_t NextPow2(uint32_t x) {
  --x;

  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;

  return ++x;
}

BitonicSorter::BitonicSorter(const char* csfile, uint32_t workgroupsize) {
  char defines[128];
  sprintf_s(defines, "#define THREADGROUP_SIZE	%d\n#define LOG2_THREADGROUP_SIZE	%d\n\n", workgroupsize,
            Log2OfPow2(workgroupsize));

  GLCreateComputeProgramFromFile(csfile, &computeshader, defines);
  threadgroupsize = workgroupsize;
}

BitonicSorter::~BitonicSorter() { delete computeshader; }

void BitonicSorter::Sort(GLuint buffer, int count, bool ascending) {
  // NOTE: optimized version

  int pow2count = NextPow2(count);
  int log2threadgroupsize = Log2OfPow2(threadgroupsize);
  int log2count = Log2OfPow2(pow2count);

  GLuint funcindex = 0;
  GLuint numthreadgroups = pow2count / threadgroupsize;

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);

  computeshader->SetInt("numValues", count);
  computeshader->SetInt("ascending", (ascending ? 1 : 0));
  computeshader->Begin();

  // presort up to k = threadgroupsize
  glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
  glDispatchCompute(numthreadgroups, 1, 1);

  // must synchronize incoherent writes
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // progressive sort from k = 2 * threadgroupsize
  for (int s = log2threadgroupsize + 1; s <= log2count; ++s) {
    computeshader->SetInt("outerIndex", s);

    for (int t = s - 1; t >= log2threadgroupsize; --t) {
      funcindex = 1;

      computeshader->SetInt("innerIndex", t);
      computeshader->CommitChanges();

      glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
      glDispatchCompute(numthreadgroups, 1, 1);

      // must synchronize incoherent writes
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // we can sort this part more efficiently
    funcindex = 2;

    glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    glDispatchCompute(numthreadgroups, 1, 1);

    // must synchronize incoherent writes
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  computeshader->End();
}

void BitonicSorter::TEST_Presort(GLuint buffer, int count, bool ascending) {
  GLuint funcindex = 0;
  int pow2count = NextPow2(count);

  computeshader->SetInt("numValues", count);
  computeshader->SetInt("ascending", (ascending ? 1 : 0));
  computeshader->Begin();
  {
    glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    glDispatchCompute(pow2count / threadgroupsize, 1, 1);
  }
  computeshader->End();

  // must synchronize incoherent writes
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void BitonicSorter::TEST_Progressive(GLuint buffer, int count, bool ascending) {
  GLuint funcindex;
  int log2threadgroupsize = Log2OfPow2(threadgroupsize);
  int pow2count = NextPow2(count);
  int log2count = Log2OfPow2(pow2count);

  computeshader->SetInt("ascending", (ascending ? 1 : 0));

  for (int s = log2threadgroupsize + 1; s <= log2count; ++s) {
    for (int t = s - 1; t >= log2threadgroupsize; --t) {
      funcindex = 1;

      computeshader->SetInt("outerIndex", s);
      computeshader->SetInt("innerIndex", t);
      computeshader->Begin();
      {
        glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
        glDispatchCompute(pow2count / threadgroupsize, 1, 1);
      }
      computeshader->End();

      // must synchronize incoherent writes
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // we can sort this part more effectively
    funcindex = 2;

    computeshader->SetInt("outerIndex", s);
    computeshader->Begin();
    {
      glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &funcindex);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
      glDispatchCompute(pow2count / threadgroupsize, 1, 1);
    }
    computeshader->End();

    // must synchronize incoherent writes
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }
}


OpenGLEffect::OpenGLEffect() {
  floatvalues = nullptr;
  intvalues = nullptr;
  uintvalues = nullptr;

  floatcap = 0;
  floatsize = 0;
  intcap = 0;
  intsize = 0;
  uintcap = 0;
  uintsize = 0;
  program = 0;
}

OpenGLEffect::~OpenGLEffect() { Destroy(); }

void OpenGLEffect::Destroy() {
  if (floatvalues != nullptr) delete[] floatvalues;

  if (intvalues != nullptr) delete[] intvalues;

  if (uintvalues != nullptr) delete[] uintvalues;

  floatvalues = nullptr;
  intvalues = nullptr;
  uintvalues = nullptr;

  if (program) glDeleteProgram(program);

  program = 0;
}

template <int size>
static char* CopyString(char (&dst)[size], const char* src) {
#ifdef _MSC_VER
  strcpy_s(dst, size, src);
  return dst;
#else
  return strcpy(dst, src);
#endif
}

void OpenGLEffect::AddUniform(const char* name, GLuint location, GLuint count, GLenum type) {
  Uniform uni;

  if (strlen(name) >= sizeof(uni.Name)) throw std::length_error("Uniform name too long");

  CopyString(uni.Name, name);

  if (type == GL_FLOAT_MAT4) count = 4;

  uni.Type = type;
  uni.RegisterCount = count;
  uni.Location = location;
  uni.Changed = true;

  if (type == GL_FLOAT || (type >= GL_FLOAT_VEC2 && type <= GL_FLOAT_VEC4) || type == GL_FLOAT_MAT4) {
    uni.StartRegister = floatsize;

    if (floatsize + count > floatcap) {
      uint32_t newcap = std::max<uint32_t>(floatsize + count, floatsize + 8);

      floatvalues = (float*)realloc(floatvalues, newcap * 4 * sizeof(float));
      floatcap = newcap;
    }

    float* reg = (floatvalues + uni.StartRegister * 4);

    if (type == GL_FLOAT_MAT4)
      MatrixIdentity((Matrix&)*reg);
    else
      memset(reg, 0, uni.RegisterCount * 4 * sizeof(float));

    floatsize += count;
  } else if (type == GL_INT || (type >= GL_INT_VEC2 && type <= GL_INT_VEC4) || type == GL_SAMPLER_2D ||
             type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_BUFFER || type == GL_SAMPLER_CUBE ||
             type == GL_IMAGE_2D) {
    uni.StartRegister = intsize;

    if (intsize + count > intcap) {
      uint32_t newcap = std::max<uint32_t>(intsize + count, intsize + 8);

      intvalues = (int*)realloc(intvalues, newcap * 4 * sizeof(int));
      intcap = newcap;
    }

    int* reg = (intvalues + uni.StartRegister * 4);
    memset(reg, 0, uni.RegisterCount * 4 * sizeof(int));

    intsize += count;
  } else if (type == GL_UNSIGNED_INT_VEC4) {
    uni.StartRegister = uintsize;

    if (uintsize + count > uintcap) {
      uint32_t newcap = std::max<uint32_t>(uintsize + count, uintsize + 8);

      uintvalues = (GLuint*)realloc(uintvalues, newcap * 4 * sizeof(GLuint));
      uintcap = newcap;
    }

    GLuint* reg = (uintvalues + uni.StartRegister * 4);
    memset(reg, 0, uni.RegisterCount * 4 * sizeof(GLuint));

    uintsize += count;
  } else {
    // not handled
    throw std::invalid_argument("This uniform type is not supported");
  }

  uniforms.Insert(uni);
}

void OpenGLEffect::AddUniformBlock(const char* name, GLint index, GLint binding, GLint blocksize) {
  UniformBlock block;

  if (strlen(name) >= sizeof(block.Name)) throw std::length_error("Uniform block name too long");

  block.Index = index;
  block.Binding = binding;
  block.BlockSize = blocksize;

  CopyString(block.Name, name);
  uniformblocks.Insert(block);
}

void OpenGLEffect::BindAttributes() {
  typedef std::map<std::string, GLuint> SemanticMap;

  if (program == 0) return;

  SemanticMap attribmap;
  GLint count;
  GLenum type;
  GLint size, loc;
  GLchar attribname[256];

  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);

  attribmap["my_Position"] = GLDECLUSAGE_POSITION;
  attribmap["my_Normal"] = GLDECLUSAGE_NORMAL;
  attribmap["my_Tangent"] = GLDECLUSAGE_TANGENT;
  attribmap["my_Binormal"] = GLDECLUSAGE_BINORMAL;
  attribmap["my_Color"] = GLDECLUSAGE_COLOR;
  attribmap["my_Texcoord0"] = GLDECLUSAGE_TEXCOORD;
  attribmap["my_Texcoord1"] = GLDECLUSAGE_TEXCOORD + 10;
  attribmap["my_Texcoord2"] = GLDECLUSAGE_TEXCOORD + 11;
  attribmap["my_Texcoord3"] = GLDECLUSAGE_TEXCOORD + 12;
  attribmap["my_Texcoord4"] = GLDECLUSAGE_TEXCOORD + 13;
  attribmap["my_Texcoord5"] = GLDECLUSAGE_TEXCOORD + 14;
  attribmap["my_Texcoord6"] = GLDECLUSAGE_TEXCOORD + 15;
  attribmap["my_Texcoord7"] = GLDECLUSAGE_TEXCOORD + 16;

  for (int i = 0; i < count; ++i) {
    memset(attribname, 0, sizeof(attribname));
    glGetActiveAttrib(program, i, 256, NULL, &size, &type, attribname);

    if (attribname[0] == 'g' && attribname[1] == 'l') continue;

    loc = glGetAttribLocation(program, attribname);

    auto it = attribmap.find(attribname);

    if (loc == -1 || it == attribmap.end())
      std::cout << "Invalid attribute found. Use the my_<semantic> syntax!\n";
    else
      glBindAttribLocation(program, it->second, attribname);
  }

  // bind fragment shader output
  GLint numrts = 0;

  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &numrts);
  glBindFragDataLocation(program, 0, "my_FragColor0");

  if (numrts > 1) glBindFragDataLocation(program, 1, "my_FragColor1");

  if (numrts > 2) glBindFragDataLocation(program, 2, "my_FragColor2");

  if (numrts > 3) glBindFragDataLocation(program, 3, "my_FragColor3");

  // relink
  glLinkProgram(program);
}

void OpenGLEffect::QueryUniforms() {
  GLint count;
  GLenum type;
  GLsizei length;
  GLint size, loc;
  GLchar uniname[256];

  if (program == 0) return;

  memset(uniname, 0, sizeof(uniname));
  uniforms.Clear();

  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count);

  if (count > 0) {
    // uniform blocks
    std::vector<std::string> blocknames;
    blocknames.reserve(count);

    for (int i = 0; i < count; ++i) {
      GLsizei namelength = 0;
      char name[32];

      glGetActiveUniformBlockName(program, i, 32, &namelength, name);

      name[namelength] = 0;
      blocknames.push_back(name);
    }

    for (size_t i = 0; i < blocknames.size(); ++i) {
      GLint blocksize = 0;
      GLint index = glGetUniformBlockIndex(program, blocknames[i].c_str());

      glGetActiveUniformBlockiv(program, (GLuint)i, GL_UNIFORM_BLOCK_DATA_SIZE, &blocksize);
      AddUniformBlock(blocknames[i].c_str(), index, INT_MAX, blocksize);
    }
  } else {
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

    // uniforms
    for (int i = 0; i < count; ++i) {
      memset(uniname, 0, sizeof(uniname));

      glGetActiveUniform(program, i, 256, &length, &size, &type, uniname);
      loc = glGetUniformLocation(program, uniname);

      for (int j = 0; j < length; ++j) {
        if (uniname[j] == '[') uniname[j] = '\0';
      }

      if (loc == -1) continue;

      AddUniform(uniname, loc, size, type);
    }
  }
}

void OpenGLEffect::Introspect() {
#ifndef __APPLE__
  GLenum props1[] = {GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES};
  GLenum props2[] = {GL_ACTIVE_VARIABLES};
  GLenum props3[] = {GL_OFFSET};

  GLint count = 0;
  GLint varcount = 0;
  GLint length = 0;
  GLint values[10] = {};

  glGetProgramInterfaceiv(program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &count);

  for (GLint i = 0; i < count; ++i) {
    glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, ARRAY_SIZE(props1), props1, ARRAY_SIZE(values), &length,
                           values);

    varcount = values[1];
    std::cout << "Shader storage block (" << i << "): size = " << values[0] << " variables = " << varcount << "\n";

    glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, ARRAY_SIZE(props2), props2, ARRAY_SIZE(values), &length,
                           values);

    for (GLint j = 0; j < varcount; ++j) {
      glGetProgramResourceiv(program, GL_BUFFER_VARIABLE, values[j], ARRAY_SIZE(props3), props3, ARRAY_SIZE(values),
                             &length, values);
      std::cout << "  variable (" << j << "): offset = " << values[0] << "\n";
    }
  }
#endif
}

void OpenGLEffect::Begin() {
  glUseProgram(program);
  CommitChanges();
}

void OpenGLEffect::CommitChanges() {
  for (size_t i = 0; i < uniforms.Size(); ++i) {
    const Uniform& uni = uniforms[i];

    float* floatdata = (floatvalues + uni.StartRegister * 4);
    int* intdata = (intvalues + uni.StartRegister * 4);
    GLuint* uintdata = (uintvalues + uni.StartRegister * 4);

    if (!uni.Changed) continue;

    uni.Changed = false;

    switch (uni.Type) {
      case GL_FLOAT: glUniform1fv(uni.Location, uni.RegisterCount, floatdata); break;

      case GL_FLOAT_VEC2: glUniform2fv(uni.Location, uni.RegisterCount, floatdata); break;

      case GL_FLOAT_VEC3: glUniform3fv(uni.Location, uni.RegisterCount, floatdata); break;

      case GL_FLOAT_VEC4: glUniform4fv(uni.Location, uni.RegisterCount, floatdata); break;

      case GL_FLOAT_MAT4: glUniformMatrix4fv(uni.Location, uni.RegisterCount / 4, false, floatdata); break;

      case GL_UNSIGNED_INT_VEC4: glUniform4uiv(uni.Location, uni.RegisterCount, uintdata); break;

      case GL_INT:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_CUBE:
      case GL_SAMPLER_BUFFER:
      case GL_IMAGE_2D: glUniform1i(uni.Location, intdata[0]); break;

      default: break;
    }
  }
}

void OpenGLEffect::End() {
  // do nothing
}

void OpenGLEffect::SetMatrix(const char* name, const float* value) { SetVector(name, value); }

void OpenGLEffect::SetVector(const char* name, const float* value) {
  Uniform test;
  CopyString(test.Name, name);

  size_t id = uniforms.Find(test);

  if (id < uniforms.Size()) {
    const Uniform& uni = uniforms[id];
    float* reg = (floatvalues + uni.StartRegister * 4);

    memcpy(reg, value, uni.RegisterCount * 4 * sizeof(float));
    uni.Changed = true;
  }
}

void OpenGLEffect::SetVectorArray(const char* name, const float* values, GLsizei count) {
  Uniform test;
  CopyString(test.Name, name);

  size_t id = uniforms.Find(test);

  if (id < uniforms.Size()) {
    const Uniform& uni = uniforms[id];
    float* reg = (floatvalues + uni.StartRegister * 4);

    if (count > uni.RegisterCount) count = uni.RegisterCount;

    memcpy(reg, values, count * 4 * sizeof(float));
    uni.Changed = true;
  }
}

void OpenGLEffect::SetFloat(const char* name, float value) {
  Uniform test;
  CopyString(test.Name, name);

  size_t id = uniforms.Find(test);

  if (id < uniforms.Size()) {
    const Uniform& uni = uniforms[id];
    float* reg = (floatvalues + uni.StartRegister * 4);

    reg[0] = value;
    uni.Changed = true;
  }
}

void OpenGLEffect::SetFloatArray(const char* name, const float* values, GLsizei count) {
  Uniform test;
  CopyString(test.Name, name);

  size_t id = uniforms.Find(test);

  if (id < uniforms.Size()) {
    const Uniform& uni = uniforms[id];
    float* reg = (floatvalues + uni.StartRegister * 4);

    if (count > uni.RegisterCount) count = uni.RegisterCount;

    memcpy(reg, values, count * sizeof(float));
    uni.Changed = true;
  }
}

void OpenGLEffect::SetInt(const char* name, int value) {
  Uniform test;
  CopyString(test.Name, name);

  size_t id = uniforms.Find(test);

  if (id < uniforms.Size()) {
    const Uniform& uni = uniforms[id];
    int* reg = (intvalues + uni.StartRegister * 4);

    reg[0] = value;
    uni.Changed = true;
  }
}

void OpenGLEffect::SetUIntVector(const char* name, const GLuint* value) {
  Uniform test;
  CopyString(test.Name, name);

  size_t id = uniforms.Find(test);

  if (id < uniforms.Size()) {
    const Uniform& uni = uniforms[id];
    GLuint* reg = (uintvalues + uni.StartRegister * 4);

    memcpy(reg, value, uni.RegisterCount * 4 * sizeof(GLuint));
    uni.Changed = true;
  }
}

void OpenGLEffect::SetUniformBlockBinding(const char* name, GLint binding) {
  UniformBlock temp;
  CopyString(temp.Name, name);

  size_t id = uniformblocks.Find(temp);

  if (id < uniformblocks.Size()) {
    const UniformBlock& block = uniformblocks[id];

    block.Binding = binding;
    glUniformBlockBinding(program, block.Index, binding);
  }
}

Matrix& Matrix::operator=(const Matrix& other) {
  _11 = other._11;
  _21 = other._21;
  _12 = other._12;
  _22 = other._22;
  _13 = other._13;
  _23 = other._23;
  _14 = other._14;
  _24 = other._24;

  _31 = other._31;
  _41 = other._41;
  _32 = other._32;
  _42 = other._42;
  _33 = other._33;
  _43 = other._43;
  _34 = other._34;
  _44 = other._44;

  return *this;
}

Matrix::Matrix() {
  _11 = _12 = _13 = _14 = 0;
  _21 = _22 = _23 = _24 = 0;
  _31 = _32 = _33 = _34 = 0;
  _41 = _42 = _43 = _44 = 0;
}
void MatrixIdentity(Matrix& out) {
    out._12 = out._13 = out._14 = 0;
    out._21 = out._23 = out._24 = 0;
    out._31 = out._32 = out._34 = 0;
    out._41 = out._42 = out._43 = 0;

    out._11 = out._22 = out._33 = out._44 = 1;
}