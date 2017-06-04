#ifndef MATERIAL_H
#define MATERIAL_H

#include "gl_headers.h"
#include "vector.h"

class Material {
	
public:
	Material(){}
	Material(Vec3f ambient, Vec3f specular, Vec3f diffuse, GLfloat shine) :
		m_ambient(ambient),
		m_specular(specular),
		m_diffuse(diffuse),
		m_shininess(shine) {}
		
	Vec3f m_ambient;
	Vec3f m_specular;
	Vec3f m_diffuse;
	GLfloat m_shininess;
};


#endif