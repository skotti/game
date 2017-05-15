#include "texture.h"
#include "debug.h"

Texture::Texture(const std::string& file_name)
{
	GL_CHECK(glEnable(GL_TEXTURE_2D));

	GL_CHECK(glGenTextures(1, &m_texture_id));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture_id));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	int image_width, image_height;
	unsigned char* image = SOIL_load_image(file_name.c_str(), &image_width, &image_height, 0, SOIL_LOAD_RGB);
	ASSERT(image);
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image));
	//glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); 
}
