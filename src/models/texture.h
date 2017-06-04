#ifndef TEXTURE_H
#define TEXTURE_H

#include "stl_headers.h"
#include "gl_headers.h"

class Texture {
	
public:
	Texture(const std::string& file_name);
	GLuint getGLid(){ return m_texture_id; }
	
	std::string getTextureName() {return m_texture_name;}
	
	void setTextureName(std::string texture_name) {m_texture_name = texture_name;}
	
private:
	GLuint m_texture_id;
	std::string m_texture_name;
};



#endif