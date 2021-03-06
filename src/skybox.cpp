#include "skybox.h"
#include "gl_headers.h"
#include "debug.h"
#include "stl_headers.h"

#include "stb_image.h"

SkyBox::SkyBox()
{
	
			GLfloat skyVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
		
		GL_CHECK(glGenVertexArrays(1, &m_sky_vao));
    GL_CHECK(glGenBuffers(1, &m_sky_vbo));
    GL_CHECK(glBindVertexArray(m_sky_vao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_sky_vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW));

    // Position attribute
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glBindVertexArray(0)); // Unbind skyVAO
		
		
    
    GL_CHECK(glGenTextures(1, &m_skybox_texture));	
    int width,height;
    unsigned char* image;
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture));
    
    std::vector<const GLchar*> faces;
    faces.push_back("textures/envmap_violentdays/violentdays_rt.tga");
    faces.push_back("textures/envmap_violentdays/violentdays_lf.tga");
    faces.push_back("textures/envmap_violentdays/violentdays_up.tga");
    faces.push_back("textures/envmap_violentdays/violentdays_dn.tga");
    faces.push_back("textures/envmap_violentdays/violentdays_bk.tga");
    faces.push_back("textures/envmap_violentdays/violentdays_ft.tga");
    
    for (int i = 0; i < faces.size(); i++) {
        //image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
				
				image = stbi_load(faces[i], &width, &height, nullptr, STBI_rgb_alpha);
				VASSERT(image, "Can't load skybox texture");
				
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
				stbi_image_free(image);
    }
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
		
}
