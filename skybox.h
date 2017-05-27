#ifndef SKYBOX_H
#define SKYBOX_H
#include "gl_headers.h"

class SkyBox {

public:
	SkyBox();
	
	GLuint getVAO() {
		return m_sky_vao;
	}
	
	GLuint getText() {
		return m_skybox_texture;
	}
	
private:
	
	GLuint m_sky_vao;
	GLuint m_sky_vbo;
	GLuint m_skybox_texture;
	
};



#endif