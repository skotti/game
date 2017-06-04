#include "texture.h"
#include "debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Texture::Texture(const std::string& file_name)
{
//	GL_CHECK(glEnable(GL_TEXTURE_2D));

	GL_CHECK(glGenTextures(1, &m_texture_id));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture_id));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
// 	glPixelStorei(GL_PACK_ALIGNMENT, 1);
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//for bad aligned textures
	int image_width, image_height;
	//unsigned char* image = SOIL_load_image(file_name.c_str(), &image_width, &image_height, 0, SOIL_LOAD_RGB);
	stbi_set_flip_vertically_on_load(true); // origin in leftmost bottom corner
	unsigned char* image = stbi_load(file_name.c_str(), &image_width, &image_height, nullptr, STBI_rgb_alpha);
	ASSERT(image);
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
	//glGenerateMipmap(GL_TEXTURE_2D);
	//SOIL_free_image_data(image);
	stbi_image_free(image);
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); 
}
