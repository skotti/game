#ifndef VECTOR_H
#define VECTOR_H

#include "stl_headers.h"
#include "gl_headers.h"

typedef std::array<long, 2> Vec2i;
typedef std::array<long, 3> Vec3i;
typedef std::array<long, 4> Vec4i;

typedef std::array<float, 2> Vec2f;
typedef std::array<float, 3> Vec3f;
typedef std::array<float, 4> Vec4f;

template <typename Type>
using Vec3 = std::array<Type, 3>;
template <typename Type>
using Vec2 = std::array<Type, 2>;

inline Vec3f toVec3f(glm::vec3 vec) {
	return Vec3f{vec.x, vec.y, vec.z};
}

inline glm::vec3 toGLMvec3(Vec3f vec) {
	return glm::vec3(vec[0], vec[1], vec[2]);
}

#endif