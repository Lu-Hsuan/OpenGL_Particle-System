#pragma once
#undef __gl_h_
#include <glad/glad.h>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include<string>
#include<map>

#ifndef _ORDEREDARRAY_HPP_
#define _ORDEREDARRAY_HPP_

#include <algorithm>
#include <cstdint>
#include <functional>
#include <ostream>
#include "pipline.h"
struct Matrix {
  // NOTE: row-major (multiply with it from the right)

  float _11, _12, _13, _14;
  float _21, _22, _23, _24;
  float _31, _32, _33, _34;
  float _41, _42, _43, _44;  // translation goes here

  Matrix();
  Matrix(const Matrix& other);
  Matrix(float v11, float v22, float v33, float v44);
  Matrix(float v11,
         float v12,
         float v13,
         float v14,
         float v21,
         float v22,
         float v23,
         float v24,
         float v31,
         float v32,
         float v33,
         float v34,
         float v41,
         float v42,
         float v43,
         float v44);
  Matrix(const float* values);

  Matrix& operator=(const Matrix& other);

  inline operator float*() { return &_11; }
  inline operator const float*() const { return &_11; }

  inline float* operator[](int row) { return (&_11 + 4 * row); }
  inline const float* operator[](int row) const { return (&_11 + 4 * row); }
};
void MatrixIdentity(Matrix& out);
enum OpenGLDeclUsage {
  GLDECLUSAGE_POSITION = 0,
  GLDECLUSAGE_BLENDWEIGHT,
  GLDECLUSAGE_BLENDINDICES,
  GLDECLUSAGE_NORMAL,
  GLDECLUSAGE_PSIZE,
  GLDECLUSAGE_TEXCOORD,
  GLDECLUSAGE_TANGENT = GLDECLUSAGE_TEXCOORD + 8,
  GLDECLUSAGE_BINORMAL,
  GLDECLUSAGE_TESSFACTOR,
  GLDECLUSAGE_POSITIONT,
  GLDECLUSAGE_COLOR,
  GLDECLUSAGE_FOG,
  GLDECLUSAGE_DEPTH,
  GLDECLUSAGE_SAMPLE
};
template <typename value_type, typename compare = std::less<value_type> >
class OrderedArray {
 private:
  value_type* data;
  size_t capacity;
  size_t size;

  size_t _find(const value_type& value) const;

 public:
  typedef std::pair<size_t, bool> pairib;

  static const size_t npos = SIZE_MAX;

  compare comp;

  OrderedArray();
  OrderedArray(const OrderedArray& other);
  ~OrderedArray();

  void Clear();
  void Destroy();
  void Erase(const value_type& value);
  void EraseAt(size_t index);
  void FastCopy(const OrderedArray& other);
  void Reserve(size_t newcap);
  void Swap(OrderedArray& other);

  pairib Insert(const value_type& value);

  size_t Find(const value_type& value) const;
  size_t LowerBound(const value_type& value) const;
  size_t UpperBound(const value_type& value) const;

  OrderedArray& operator=(const OrderedArray& other);

  inline const value_type& operator[](size_t index) const { return data[index]; }

  inline size_t Size() const { return size; }

  inline size_t Capacity() const { return capacity; }

  template <typename T, typename U>
  friend std::ostream& operator<<(std::ostream& os, const OrderedArray<T, U>& arr);
};

#endif

class OpenGLEffect {
  friend bool GLCreateEffectFromFile(
      const char*, const char*, const char*, const char*, const char*, OpenGLEffect**, const char*);
  // friend bool GLCreateEffectFromMemory(const char*, const char*, const char*, const char*, const char*,
  // OpenGLEffect**, const char*);
  friend bool GLCreateComputeProgramFromFile(const char*, OpenGLEffect**, const char*);

  struct Uniform {
    char Name[32];
    GLint StartRegister;
    GLint RegisterCount;
    GLint Location;
    GLenum Type;

    mutable bool Changed;

    inline bool operator<(const Uniform& other) const { return (0 > strcmp(Name, other.Name)); }
  };

  struct UniformBlock {
    char Name[32];
    GLint Index;
    mutable GLint Binding;
    GLint BlockSize;

    inline bool operator<(const UniformBlock& other) const { return (0 > strcmp(Name, other.Name)); }
  };

  typedef OrderedArray<Uniform> UniformTable;
  typedef OrderedArray<UniformBlock> UniformBlockTable;

 private:
  UniformTable uniforms;
  UniformBlockTable uniformblocks;
  GLuint program;

  float* floatvalues;
  int* intvalues;
  GLuint* uintvalues;

  uint32_t floatcap;
  uint32_t floatsize;
  uint32_t intcap;
  uint32_t intsize;
  uint32_t uintcap;
  uint32_t uintsize;

  OpenGLEffect();

  void AddUniform(const char* name, GLuint location, GLuint count, GLenum type);
  void AddUniformBlock(const char* name, GLint index, GLint binding, GLint blocksize);
  void BindAttributes();
  void Destroy();
  void QueryUniforms();

 public:
  ~OpenGLEffect();

  void Begin();
  void CommitChanges();
  void End();
  void Introspect();

  void SetMatrix(const char* name, const float* value);
  void SetVector(const char* name, const float* value);
  void SetVectorArray(const char* name, const float* values, GLsizei count);
  void SetFloat(const char* name, float value);
  void SetFloatArray(const char* name, const float* values, GLsizei count);
  void SetInt(const char* name, int value);
  void SetUIntVector(const char* name, const GLuint* value);
  void SetUniformBlockBinding(const char* name, GLint binding);
};

class BitonicSorter {
 private:
  OpenGLEffect* computeshader;
  uint32_t threadgroupsize;

 public:
  BitonicSorter(const char* csfile, uint32_t workgroupsize);
  ~BitonicSorter();

  void Sort(GLuint buffer, int count, bool ascending);

  // utility methods for testing
  void TEST_Presort(GLuint buffer, int count, bool ascending);
  void TEST_Progressive(GLuint buffer, int count, bool ascending);

  inline uint32_t GetThreadGroupSize() const { return threadgroupsize; }
  inline OpenGLEffect* GetShader() const { return computeshader; }
};
