#ifndef TEXTURE_H
#define TEXTURE_H

#include "stl_headers.h"
#include "gl_headers.h"

class Texture {
	
public:
	Texture(const std::string& file_name);
	GLuint getGLid(){ return m_texture_id; }
	
private:
	GLuint m_texture_id;
};



#endif