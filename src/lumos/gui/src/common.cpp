#include "lumos/gui/common.h"
#include "lumos/core/exception.h"
#include <exception>

#define NAME_PAIR(x)                                                           \
  { x, #x }

#define TYPE_NAME_PAIR(x, y)                                                   \
  { x, #y }

namespace lumos {
namespace gui {
const std::string &GetTypeName(GLenum type) {
  static std::unordered_map<GLint, std::string> map{
      TYPE_NAME_PAIR(GL_FLOAT, float), TYPE_NAME_PAIR(GL_FLOAT_VEC2, vec2),
      TYPE_NAME_PAIR(GL_FLOAT_VEC3, vec3), TYPE_NAME_PAIR(GL_FLOAT_VEC4, vec4),
      TYPE_NAME_PAIR(GL_DOUBLE, double),
      // TYPE_NAME_PAIR(GL_DOUBLE_VEC2, dvec2),
      // TYPE_NAME_PAIR(GL_DOUBLE_VEC3, dvec3),
      // TYPE_NAME_PAIR(GL_DOUBLE_VEC4, dvec4),
      TYPE_NAME_PAIR(GL_INT, int), TYPE_NAME_PAIR(GL_INT_VEC2, ivec2),
      TYPE_NAME_PAIR(GL_INT_VEC3, ivec3), TYPE_NAME_PAIR(GL_INT_VEC4, ivec4),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT, unsigned int),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_VEC2, uvec2),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_VEC3, uvec3),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_VEC4, uvec4),
      TYPE_NAME_PAIR(GL_BOOL, bool), TYPE_NAME_PAIR(GL_BOOL_VEC2, bvec2),
      TYPE_NAME_PAIR(GL_BOOL_VEC3, bvec3), TYPE_NAME_PAIR(GL_BOOL_VEC4, bvec4),
      TYPE_NAME_PAIR(GL_FLOAT_MAT2, mat2), TYPE_NAME_PAIR(GL_FLOAT_MAT3, mat3),
      TYPE_NAME_PAIR(GL_FLOAT_MAT4, mat4),
      TYPE_NAME_PAIR(GL_FLOAT_MAT2x3, mat2x3),
      TYPE_NAME_PAIR(GL_FLOAT_MAT2x4, mat2x4),
      TYPE_NAME_PAIR(GL_FLOAT_MAT3x2, mat3x2),
      TYPE_NAME_PAIR(GL_FLOAT_MAT3x4, mat3x4),
      TYPE_NAME_PAIR(GL_FLOAT_MAT4x2, mat4x2),
      TYPE_NAME_PAIR(GL_FLOAT_MAT4x3, mat4x3),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT2, dmat2),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT3, dmat3),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT4, dmat4),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT2x3, dmat2x3),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT2x4, dmat2x4),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT3x2, dmat3x2),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT3x4, dmat3x4),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT4x2, dmat4x2),
      // TYPE_NAME_PAIR(GL_DOUBLE_MAT4x3, dmat4x3),
      TYPE_NAME_PAIR(GL_SAMPLER_1D, sampler1D),
      TYPE_NAME_PAIR(GL_SAMPLER_2D, sampler2D),
      TYPE_NAME_PAIR(GL_SAMPLER_3D, sampler3D),
      TYPE_NAME_PAIR(GL_SAMPLER_CUBE, samplerCube),
      TYPE_NAME_PAIR(GL_SAMPLER_1D_SHADOW, sampler1DShadow),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_SHADOW, sampler2DShadow),
      TYPE_NAME_PAIR(GL_SAMPLER_1D_ARRAY, sampler1DArray),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_ARRAY, sampler2DArray),
      TYPE_NAME_PAIR(GL_SAMPLER_1D_ARRAY_SHADOW, sampler1DArrayShadow),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_ARRAY_SHADOW, sampler2DArrayShadow),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_MULTISAMPLE, sampler2DMS),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_MULTISAMPLE_ARRAY, sampler2DMSArray),
      TYPE_NAME_PAIR(GL_SAMPLER_CUBE_SHADOW, samplerCubeShadow),
      TYPE_NAME_PAIR(GL_SAMPLER_BUFFER, samplerBuffer),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_RECT, sampler2DRect),
      TYPE_NAME_PAIR(GL_SAMPLER_2D_RECT_SHADOW, sampler2DRectShadow),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_1D, isampler1D),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_2D, isampler2D),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_3D, isampler3D),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_CUBE, isamplerCube),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_1D_ARRAY, isampler1DArray),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_2D_ARRAY, isampler2DArray),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_2D_MULTISAMPLE, isampler2DMS),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, isampler2DMSArray),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_BUFFER, isamplerBuffer),
      TYPE_NAME_PAIR(GL_INT_SAMPLER_2D_RECT, isampler2DRect),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_1D, usampler1D),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D, usampler2D),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_3D, usampler3D),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_CUBE, usamplerCube),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, usampler2DArray),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, usampler2DArray),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, usampler2DMS),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
                     usampler2DMSArray),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_BUFFER, usamplerBuffer),
      TYPE_NAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_RECT, usampler2DRect),
      // TYPE_NAME_PAIR(GL_IMAGE_1D, image1D),
      // TYPE_NAME_PAIR(GL_IMAGE_2D, image2D),
      // TYPE_NAME_PAIR(GL_IMAGE_3D, image3D),
      // TYPE_NAME_PAIR(GL_IMAGE_2D_RECT, image2DRect),
      // TYPE_NAME_PAIR(GL_IMAGE_CUBE, imageCube),
      // TYPE_NAME_PAIR(GL_IMAGE_BUFFER, imageBuffer),
      // TYPE_NAME_PAIR(GL_IMAGE_1D_ARRAY, image1DArray),
      // TYPE_NAME_PAIR(GL_IMAGE_2D_ARRAY, image2DArray),
      // TYPE_NAME_PAIR(GL_IMAGE_2D_MULTISAMPLE, image2DMS),
      // TYPE_NAME_PAIR(GL_IMAGE_2D_MULTISAMPLE_ARRAY, image2DMSArray),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_1D, iimage1D),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_2D, iimage2D),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_3D, iimage3D),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_2D_RECT, iimage2DRect),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_CUBE, iimageCube),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_BUFFER, iimageBuffer),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_1D_ARRAY, iimage1DArray),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_2D_ARRAY, iimage2DArray),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_2D_MULTISAMPLE, iimage2DMS),
      // TYPE_NAME_PAIR(GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY, iimage2DMSArray),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_1D, uimage1D),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D, uimage2D),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_3D, uimage3D),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_RECT, uimage2DRect),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_CUBE, uimageCube),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_BUFFER, uimageBuffer),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_1D_ARRAY, uimage1DArray),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_ARRAY, uimage2DArray),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, uimage2DMS),
      // TYPE_NAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
      // uimage2DMSArray), TYPE_NAME_PAIR(GL_UNSIGNED_INT_ATOMIC_COUNTER,
      // atomic_uint),
  };
  if (map.find(type) == map.end()) {
    throw RuntimeError("Unknown OpenGL type: {}", type);
  }
  return map[type];
}

const std::string &GetErrorTypeName(GLenum error_type) {

  static std::unordered_map<GLint, std::string> map{
      NAME_PAIR(GL_INVALID_ENUM),
      NAME_PAIR(GL_INVALID_VALUE),
      NAME_PAIR(GL_INVALID_OPERATION),
      NAME_PAIR(GL_INVALID_FRAMEBUFFER_OPERATION),
      NAME_PAIR(GL_OUT_OF_MEMORY),
#ifdef GL_STACK_UNDERFLOW
      NAME_PAIR(GL_STACK_UNDERFLOW),
#endif
#ifdef GL_STACK_OVERFLOW
      NAME_PAIR(GL_STACK_OVERFLOW),
#endif
  };
  if (map.find(error_type) == map.end()) {
    throw RuntimeError("Unknown OpenGL Error type: {}", error_type);
  }
  return map[error_type];
}

static const std::string &getShaderTypeName(GLint shader_type) {
  static std::unordered_map<GLint, std::string> map{
      NAME_PAIR(GL_VERTEX_SHADER),
      NAME_PAIR(GL_FRAGMENT_SHADER),
      NAME_PAIR(GL_GEOMETRY_SHADER),
  };
  if (map.find(shader_type) == map.end()) {
    throw RuntimeError("Unknown OpenGL Shader type: {}", shader_type);
  }
  return map[shader_type];
}

void CheckOpenGLShaderErrors(GLuint shader) {
  GLint shader_type;
  GLint info_log_length{};
  GLint success{};
  std::vector<char> info_log;
  // Get shader info log from opengl
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success) {
    return;
  }
  CheckOpenGLErrors();
  glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > info_log.size()) {
    info_log.reserve(info_log_length);
  }
  info_log.resize(info_log_length);
  glGetShaderInfoLog(shader, info_log_length, nullptr, info_log.data());
  throw RuntimeError("OpenGL Shader [type={}] error:\n{}",
                     getShaderTypeName(shader_type), info_log.data());
}

void CheckOpenGLProgramErrors(GLuint program) {
  GLint info_log_length{};
  GLint link_status{};
  std::vector<char> info_log;
  // Get shader info log from opengl
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status == GL_TRUE) {
    return;
  }
  CheckOpenGLErrors();
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > info_log.size()) {
    info_log.reserve(info_log_length);
  }
  info_log.resize(info_log_length);
  glGetProgramInfoLog(program, info_log_length, nullptr, info_log.data());
  throw RuntimeError("OpenGL Program error:\n{}", info_log.data());
}

template <>
void SetUniform(GLuint program, std::string name, const Eigen::Matrix4f &data) {
  GLint location = glGetUniformLocation(program, name.c_str());
  glUniformMatrix4fv(location, 1, data.IsRowMajor ? GL_FALSE : GL_TRUE,
                     data.data());
  CheckOpenGLErrors();
}

} // namespace gui
} // namespace lumos