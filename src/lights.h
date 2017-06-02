#ifndef LIGHTS_H
#define LIGHTS_H

#include "gl_headers.h"
#include "vector.h"

class Light {

public:
	Light(){}
	
	Light(Vec3f direction, Vec3f ambient, Vec3f specular, Vec3f diffuse, int type) :
		m_direction(direction),
		m_ambient(ambient),
		m_specular(specular),
		m_diffuse(diffuse),
		m_type(type) {
			m_position = Vec3f{-2.0f, 4.0f, -1.0f};
		};
		
	Light(Vec3f position, Vec3f ambient, Vec3f specular, Vec3f diffuse, float constant, float linear, float quadratic, int type) :
		m_position(position),
		m_ambient(ambient),
		m_specular(specular),
		m_diffuse(diffuse),
		m_constant(constant),
		m_linear(linear),
		m_quadratic(quadratic),
		m_type(type) {};
	
	Vec3f m_position;
	
	float m_constant;
	float m_linear;
	float m_quadratic;
	
	Vec3f m_ambient;
	Vec3f m_diffuse;
	Vec3f m_specular;
	
	Vec3f m_direction;
	int m_type;
	
};

#endif

