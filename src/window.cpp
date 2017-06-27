#include "window.h"
#include "debug.h"

#include "logger.h"
#include "gl_headers.h"
#include "stl_headers.h"

#include "engine.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

const GLuint Window::S_WIDTH = 1280;
const GLuint Window::S_HEIGHT = 960;

const float Window::S_Z_NEAR = 0.1;
const float Window::S_Z_FAR = 20.0; 
const float Window::S_FOV_Y = 45.0 / 180.0 * M_PI;//45.0;

const GLuint Window::S_SHADOW_WIDTH = 1920;
const GLuint Window::S_SHADOW_HEIGHT = 1080;

std::map<std::string, Texture*> Window::S_TEXTURES;
GLuint quadVAO;
GLuint quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -0.9f,  0.9f, 0.0f,  0.0f, 1.0f,
            -0.9f, -0.9f, 0.0f,  0.0f, 0.0f,
             0.9f,  0.9f, 0.0f,  1.0f, 1.0f,
             0.9f, -0.9f, 0.0f,  1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    GL_CHECK(glBindVertexArray(quadVAO));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GL_CHECK(glBindVertexArray(0));
}

Window::Window() {
	m_logger.log("Create window");
	
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	m_window = glfwCreateWindow(S_WIDTH, S_HEIGHT, "Cube Game", nullptr, nullptr);
	ASSERT(m_window != nullptr);
	glfwMakeContextCurrent(m_window);
	
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	while(glGetError()) {} // eat error produced by glew

	GL_CHECK(glViewport(0, 0, S_WIDTH, S_HEIGHT));
	
	glEnable(GL_MULTISAMPLE);  
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthFunc(GL_LESS));
	
	// to draw fonts
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	createVisualObjects();
	
	createObjectShader();
	createDepthShader();
	createFontShader();
	createShadowShader();
	createSkyBoxShader();
	createInterShader();
	
	// create light source
// 	Vec3f light_direction = {1.0, -1.0, 1.0};
// 	Vec3f light_ambient = {0.1f, 0.1f, 0.1f};
// 	Vec3f light_diffuse = {0.8f, 0.8f, 0.8f};
// 	Vec3f light_specular = {0.5, 0.5, 0.5};
// 	int light_type = 1;
	
	initFonts();
	initTextObject();

	m_shadow = new Shadow();
	m_skybox = new SkyBox();
}

Window::~Window() {
	ASSERT(m_object_shader != nullptr);
	delete m_object_shader;
	ASSERT(m_depth_shader != nullptr)
	delete m_depth_shader;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Window::addTexture(const std::string& file_name) {
	if (S_TEXTURES.find(file_name) != S_TEXTURES.end())
		S_TEXTURES[file_name] = new Texture(file_name);
}


int Window::registerGameObject(const std::string& window_model_name) {
	View* view = new View();
	VASSERT(m_objects.count(window_model_name) == 1, "Unknown model name");
	view->m_object = m_objects.at(window_model_name);
	m_view.push_back(view);
	int id = objectIdCounter++;
	return id;
}

void Window::destroyGameObject(int id)
{
	ASSERT(m_view.at(id) != nullptr);
	delete m_view.at(id);
	m_view.at(id) = nullptr;
}

void Window::createVisualObjects() {
	m_objects["cube"] = new Object("models/cube_textured.obj");
	//m_objects["dynamic_enemy"] = new Object("models/eyeball.obj");
	m_objects["wall"] = new Object("models/wall.obj");
}

void Window::createObjectShader() {
	m_object_shader = new ShaderProgram();
	Shader vertex_shader("shaders/material_vertex.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/material_fragment.frag", ShaderType::FRAGMENT);
	
	m_object_shader->attach(&vertex_shader);
	m_object_shader->attach(&fragment_shader);
	m_object_shader->compile();
	
}

void Window::createDepthShader() {
	m_depth_shader = new ShaderProgram();
	Shader vertex_shader("shaders/cascade_depth.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/cascade_depth.frag", ShaderType::FRAGMENT);
	
	m_depth_shader->attach(&vertex_shader);
	m_depth_shader->attach(&fragment_shader);
	m_depth_shader->compile();
	
}

void Window::createFontShader() {
	m_font_shader = new ShaderProgram();
	Shader vertex_shader("shaders/text_vertex.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/text_fragment.frag", ShaderType::FRAGMENT);
	
	m_font_shader->attach(&vertex_shader);
	m_font_shader->attach(&fragment_shader);
	m_font_shader->compile();
}

void Window::createShadowShader() {
	m_shadow_shader = new ShaderProgram();
	Shader vertex_shader("shaders/cascade_shadow.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/cascade_shadow.frag", ShaderType::FRAGMENT);
	
	m_shadow_shader->attach(&vertex_shader);
	m_shadow_shader->attach(&fragment_shader);
	m_shadow_shader->compile();
	
}

void Window::createSkyBoxShader() {
	m_skybox_shader = new ShaderProgram();
	Shader vertex_shader("shaders/skybox_vertex.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/skybox_fragment.frag", ShaderType::FRAGMENT);
	
	m_skybox_shader->attach(&vertex_shader);
	m_skybox_shader->attach(&fragment_shader);
	m_skybox_shader->compile();
	
}

void Window::createInterShader() {
	m_inter_shader = new ShaderProgram();
	Shader vertex_shader("shaders/test_depth.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/test_depth.frag", ShaderType::FRAGMENT);
	
	m_inter_shader->attach(&vertex_shader);
	m_inter_shader->attach(&fragment_shader);
	m_inter_shader->compile();
}

void Window::draw() {
	GameState state = Engine::instance()->getGameState();
	if (state == GameState::Game || state == GameState::GameMenu) {

	int view_size = m_view.size();
	glm::vec3 camera_pos = toGLMvec3(getCamera().getPos());
	glm::vec3 camera_front = toGLMvec3(getCamera().getFront());
	glm::vec3 camera_up = toGLMvec3(getCamera().getUp());
	
	GLuint program = m_depth_shader->getShaderProgram();
	GL_CHECK(glUseProgram(program));
	glViewport(0, 0, Window::S_SHADOW_WIDTH, Window::S_SHADOW_HEIGHT);
	//renderShadows
	std::array<glm::mat4, Shadow::S_NUM_CASCADES> light_projection_matrix;
	std::array<glm::mat4, Shadow::S_NUM_CASCADES> light_vp_matrix; 
	glm::vec3 light_pos = glm::vec3(-1.0f, 2.0f, -1.0f);
	glm::vec3 light_dir = glm::vec3(1.4f, -0.5f, 0.8f);

	std::array<glm::vec4, 8> camera_corners;
	
	for (int i = 0; i < Shadow::S_NUM_CASCADES; i++) {

		//bind current cascade--------------------------------------------------
		m_shadow->bindForWriting(i);
		glClear(GL_DEPTH_BUFFER_BIT);
		//----------------------------------------------------------------------

		//set view camera and light matrix--------------------------------------
		glm::mat4 view_camera_matrix = glm::lookAt(camera_pos, camera_front + camera_pos, camera_up);
		glm::mat4 view_light_matrix = glm::lookAt(light_pos, light_dir + light_pos, camera_up);
		//----------------------------------------------------------------------

		//calc box coordinates for current cascade------------------------------
		if (i == 0) {
			camera_corners = m_shadow->calcOrthoProj(i, view_camera_matrix, view_light_matrix);
		} else {
			m_shadow->calcOrthoProj(i, view_camera_matrix, view_light_matrix);
		}
		ShadowBox shadow_info = m_shadow->getBox(i);
		std::cout<<i<<": far - "<<shadow_info.m_far<<", near - "<<shadow_info.m_near<<", bottom - "<<shadow_info.m_bottom<<", top - "<<shadow_info.m_top<<", left - "<<shadow_info.m_left<<", right - "<<shadow_info.m_right<<std::endl;
		//----------------------------------------------------------------------

		//set light projection matrix and light space matrix based on this box--
		light_projection_matrix.at(i) = glm::ortho(shadow_info.m_left, shadow_info.m_right, shadow_info.m_bottom, shadow_info.m_top, shadow_info.m_near, shadow_info.m_far);
		light_vp_matrix.at(i) = light_projection_matrix.at(i) * view_light_matrix;
		//---------------------------------------------------------------------
		for (int j = 0; j < m_view.size(); j++) {
		
			if (m_view.at(j) == nullptr) continue;
			
			View* view = m_view.at(j);
			Object* obj = view->m_object;
			
			for (int k = 0; k < obj->meshesNum(); k++) {
				GL_CHECK(glBindVertexArray(obj->getMesh(k)->getVAO()));

				glm::mat4 model;
				model = glm::translate(model, toGLMvec3(view->m_pos));
				model = glm::scale(model, toGLMvec3(view->m_size));
				glm::mat4 mvp = light_vp_matrix.at(i) * model;

				GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp)));
				GL_CHECK(glDrawElements(GL_TRIANGLES, obj->getMesh(k)->getIndicesSize(), GL_UNSIGNED_INT, 0));

				GL_CHECK(glBindVertexArray(0));
			}
		}
	//-----------------------------------------------------------------------
	}
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glViewport(0, 0, S_WIDTH, S_HEIGHT);  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	drawSkyBox(camera_pos, camera_front, camera_up);

	program = m_shadow_shader->getShaderProgram();
	
	GL_CHECK(glUseProgram(program));  
	m_shadow->bindForReading();
	//set current view position------------------------------------------------
	GLint view_pos_loc = GL_CHECK(glGetUniformLocation(program, "view_pos"));
	GL_CHECK(glUniform3f(view_pos_loc, camera_pos.x, camera_pos.y, camera_pos.z)); 
	//-------------------------------------------------------------------------
	//set light properties----------------------------------------------------
	int start_of_point_sources;
	GL_CHECK(glUniform3f(glGetUniformLocation(program, "dir_light.direction"), light_dir.x, light_dir.y, light_dir.z));
	GL_CHECK(glUniform3f(glGetUniformLocation(program, "dir_light.ambient"), 0.1f, 0.1f, 0.1f));
	GL_CHECK(glUniform3f(glGetUniformLocation(program, "dir_light.diffuse"), 0.8f, 0.8f, 0.8f));
	GL_CHECK(glUniform3f(glGetUniformLocation(program, "dir_light.specular"), 0.5, 0.5, 0.5));
	GL_CHECK(glUniform3f(glGetUniformLocation(program, "light_pos"), light_pos.x, light_pos.y, light_pos.z));
	//------------------------------------------------------------------------

	//set all matricies-------------------------------------------------------
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "light_view[0]"), 1, GL_FALSE, glm::value_ptr(light_vp_matrix.at(0))));
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "light_view[1]"), 1, GL_FALSE, glm::value_ptr(light_vp_matrix.at(1))));
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "light_view[2]"), 1, GL_FALSE, glm::value_ptr(light_vp_matrix.at(2))));
	//------------------------------------------------------------------------

	//set shadow maps---------------------------------------------------------
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "shadow_maps[0]"), 5/*m_shadow->getMap(0)*/));
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "shadow_maps[1]"), 6/*m_shadow->getMap(1)*/));
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "shadow_maps[2]"), 7/*m_shadow->getMap(2)*/));
	//------------------------------------------------------------------------
	

	glm::mat4 view_matrix = glm::lookAt(camera_pos, camera_front + camera_pos, camera_up);
	glm::mat4 projection = glm::perspective(Window::S_FOV_Y, (GLfloat)S_WIDTH / (GLfloat)S_HEIGHT, Window::S_Z_NEAR, Window::S_Z_FAR);
	
	std::array<glm::vec4, 8> cc_copy = camera_corners;
	
	float reverse_z_array[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
		
	glm::mat4 reverse_z = glm::make_mat4(reverse_z_array);
	for (int i = 0; i < 8; ++i) {
		camera_corners[i] = projection *  reverse_z * camera_corners[i];
	}
	
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[0]"), 1, glm::value_ptr(camera_corners[0])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[1]"), 1, glm::value_ptr(camera_corners[1])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[2]"), 1, glm::value_ptr(camera_corners[2])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[3]"), 1, glm::value_ptr(camera_corners[3])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[4]"), 1, glm::value_ptr(camera_corners[4])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[5]"), 1, glm::value_ptr(camera_corners[5])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[6]"), 1, glm::value_ptr(camera_corners[6])));
	GL_CHECK(glUniform4fv(glGetUniformLocation(program, "points[7]"), 1, glm::value_ptr(camera_corners[7])));
	//------------------------------------------------------------------------

	//set cascade ends--------------------------------------------------------
	glm::vec4 ndc_end;
	glm::vec4 clip_end;

	glm::vec4 test = glm::vec4(0.0f, 0.0f,-m_shadow->getEnd(1), 1.0f);
	clip_end = projection * glm::vec4(0.0f, 0.0f,-m_shadow->getEnd(1), 1.0f);
	ndc_end = clip_end;// / clip_end.w;
	GL_CHECK(glUniform1f(glGetUniformLocation(program, "cascade_ends[0]"), ndc_end.z));
	clip_end = projection * glm::vec4(0.0f, 0.0f,-m_shadow->getEnd(2), 1.0f);
	ndc_end = clip_end;/// clip_end.w;
	GL_CHECK(glUniform1f(glGetUniformLocation(program, "cascade_ends[1]"), ndc_end.z));
	clip_end = projection * glm::vec4(0.0f, 0.0f,-m_shadow->getEnd(3), 1.0f);
	ndc_end = clip_end;// / clip_end.w;
	GL_CHECK(glUniform1f(glGetUniformLocation(program, "cascade_ends[2]"), ndc_end.z));
	//------------------------------------------------------------------------

	GL_CHECK(glUseProgram(program));
	m_shadow->bindForReading();
	
	view_size = m_view.size();
	int text_size = S_TEXTURES.size();
	Vec3f t_ambient, t_diffuse, t_specular;
	std::cout<<"VIEW_SIZE : "<<m_view.size()<<std::endl;
	for (int j = 0; j < m_view.size(); j++) {

		if (m_view.at(j) == nullptr) continue;
		
		View* view = m_view.at(j);
		Object* obj = view->m_object;
// 		std::cout<<"j:"<<j<<", cur_pos : "<<view->m_pos[0]<<" "<<view->m_pos[1]<<" "<<view->m_pos[2]<<std::endl;
// 		std::cout<<"j:"<<j<<", cur_size : "<<view->m_size[0]<<" "<<view->m_size[1]<<" "<<view->m_size[2]<<std::endl;
			
		for (int k = 0; k < obj->meshesNum(); k++) {
			GL_CHECK(glActiveTexture(GL_TEXTURE0+1));
			
			std::string texture_name = obj->getMesh(k)->m_texture;
			Vec3f cur_pos = view->m_pos;
			Vec3f cur_size = view->m_size;
			if (obj->getMesh(k)->m_has_texture)
				glBindTexture(GL_TEXTURE_2D,S_TEXTURES.at(texture_name)->getGLid());
			GL_CHECK(glBindVertexArray(obj->getMesh(k)->getVAO()));

			glm::mat4 model;
			model = glm::translate(model, toGLMvec3(view->m_pos));
			model = glm::scale(model, toGLMvec3(view->m_size));
			model *= glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -toGLMvec3(view->m_front), toGLMvec3(view->m_up));
			
			GL_CHECK(glUniform1i(glGetUniformLocation(program, "has_texture"), obj->getMesh(k)->m_has_texture));
			GL_CHECK(glUniform1i(glGetUniformLocation(program, "texture_sampler"), 1));
			t_ambient = obj->getMesh(k)->m_material->m_ambient;
			GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.ambient"), t_ambient[0], t_ambient[1], t_ambient[2]));
			t_diffuse = obj->getMesh(k)->m_material->m_diffuse;
			GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.diffuse"), t_diffuse[0], t_diffuse[1], t_diffuse[2]));
			t_specular = obj->getMesh(k)->m_material->m_specular;
			GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.specular"), t_specular[0], t_specular[1], t_specular[2]));
			
			GL_CHECK(glUniform1f(glGetUniformLocation(program, "material.shininess"), 100));
			
			GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix)));
			GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model)));
			GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
			GL_CHECK(glDrawElements(GL_TRIANGLES, obj->getMesh(k)->getIndicesSize(), GL_UNSIGNED_INT, 0));

			GL_CHECK(glBindVertexArray(0));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
		}
	}


	} // end of long drawing code
	
	drawFonts();
}

void Window::drawSkyBox(glm::vec3 camera_pos, glm::vec3 camera_front, glm::vec3 camera_up)
{
	GLuint program = m_skybox_shader->getShaderProgram();
	
	glm::mat4 skyview = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
	glDepthMask(GL_FALSE);// Remember to turn depth writing offset
	GL_CHECK(glUseProgram(program));		
	// glm::mat4 skyview = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 skyprojection = glm::perspective(Window::S_FOV_Y, (float)Window::S_WIDTH/(float)Window::S_HEIGHT, Window::S_Z_NEAR, Window::S_Z_FAR);
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(skyview)));
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(skyprojection)));


	// skybox cube
	GL_CHECK(glBindVertexArray(m_skybox->getVAO()));
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	//        glm::mat4 skymodel;
	glm::mat4 skymodel = glm::translate(glm::mat4(), camera_pos);
	GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(skymodel)));
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "skybox"), 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox->getText()));
	GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
	GL_CHECK(glBindVertexArray(0));

	glDepthMask(GL_TRUE);
}


void Window::initFonts() {	
	FT_Library ft;
	FT_CHECK(FT_Init_FreeType(&ft));

	FT_Face face;
	FT_CHECK(FT_New_Face(ft, "fonts/DejaVuSansMono.ttf", 0, &face));
	
	FT_CHECK(FT_Set_Pixel_Sizes(face, 0, 48));
	
	GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); // use first byte of colour
	
	for (GLubyte c = 0; c < S_NUM_CHARS; c++)
	{
		FT_CHECK(FT_Load_Char(face, c, FT_LOAD_RENDER));

		GLuint& texture = m_characters.at(c).m_texture_id;
		GL_CHECK(glGenTextures(1, &texture));
		GL_CHECK(glActiveTexture(GL_TEXTURE0+2));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));
		GL_CHECK(glTexImage2D(
			GL_TEXTURE_2D, // target
			0, // level
			GL_RED, // internal format
			face->glyph->bitmap.width, // width
			face->glyph->bitmap.rows, // height
			0, // border
			GL_RED, // format 
			GL_UNSIGNED_BYTE, // type 
			face->glyph->bitmap.buffer // pixels
		));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		m_characters.at(c).m_size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		m_characters.at(c).m_bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		Logger::instance()->log("BITMAP: % % % %", (char)c, face->glyph->bitmap_top, face->glyph->bitmap.rows, (int)face->glyph->bitmap.rows-(int)face->glyph->bitmap_top);
		m_characters.at(c).m_advance = face->glyph->advance.x;
	}
	
	//GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	
	FT_CHECK(FT_Done_Face(face));
	FT_CHECK(FT_Done_FreeType(ft));
}

void Window::initTextObject()
{
	GL_CHECK(glGenVertexArrays(1, &m_text_object.m_vao));
	GL_CHECK(glGenBuffers(1, &m_text_object.m_vbo));
	
	GL_CHECK(glBindVertexArray(m_text_object.m_vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_text_object.m_vbo));
	
	const int quad_vertices = 6;
	const int floats_per_vertice = 4;
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * quad_vertices * floats_per_vertice, NULL, GL_DYNAMIC_DRAW));
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));
	
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindVertexArray(0));
}

void Window::drawFonts()
{
	const int quad_vertices = 6;
	const int floats_per_vertice = 4;
	
	GLint loc = 0;
	
	glm::mat4 projection_matrix = glm::ortho(0.0f, static_cast<GLfloat>(S_WIDTH), 0.0f, static_cast<GLfloat>(S_HEIGHT));
	GL_CHECK(glUseProgram(m_font_shader->getShaderProgram()));
	loc = GL_CHECK(glGetUniformLocation(m_font_shader->getShaderProgram(), "projection"));
	GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection_matrix)));
	
	GL_CHECK(glActiveTexture(GL_TEXTURE0+2));
	GL_CHECK(glBindVertexArray(m_text_object.m_vao));
	
	for (auto&& text : m_text) {
		if (text->m_hidden) continue;
		
		// set color
		GL_CHECK(glUseProgram(m_font_shader->getShaderProgram()));
		loc = GL_CHECK(glGetUniformLocation(m_font_shader->getShaderProgram(), "textColor"));
		GL_CHECK(glUniform3f(loc, text->m_color[0], text->m_color[1], text->m_color[2]));
		
		GL_CHECK(glUniform1i(glGetUniformLocation(m_font_shader->getShaderProgram(), "text"), 2));
		
		GLfloat x = text->m_pos[0] * S_WIDTH;
		GLfloat y = text->m_pos[1] * S_HEIGHT;
		for (auto&& c : text->m_text) {
			CharacterTexture& ct = m_characters.at(c);
			
			GLfloat x_pos = x + ct.m_bearing.x * text->m_scale * S_HEIGHT;
			GLfloat y_pos = y - (ct.m_size.y - ct.m_bearing.y) * text->m_scale * S_HEIGHT;
			                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
			GLfloat width = ct.m_size.x * text->m_scale * S_HEIGHT;
			GLfloat height = ct.m_size.y * text->m_scale * S_HEIGHT;
			
			// Update VBO for each character
			GLfloat vertices[quad_vertices][floats_per_vertice] = {
				{ x_pos,         y_pos + height,   0.0, 0.0 },            
				{ x_pos,         y_pos,            0.0, 1.0 },
				{ x_pos + width, y_pos,            1.0, 1.0 },
				
				{ x_pos,         y_pos + height,   0.0, 0.0 },
				{ x_pos + width, y_pos,            1.0, 1.0 },
				{ x_pos + width, y_pos + height,   1.0, 0.0 }           
			};
			
			// Render glyph texture over quad
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, ct.m_texture_id));
			// Update content of VBO memory
			GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_text_object.m_vbo));
			// Be sure to use glBufferSubData and not glBufferData
			GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));

			GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
			// Render quad
			GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, quad_vertices));
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			int advance_step = 64;
			x += (ct.m_advance / advance_step) * text->m_scale * S_HEIGHT; 
		}
	}
	GL_CHECK(glBindVertexArray(0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

Window::TextId Window::registerText(const std::string& text, Vec2f pos, float scale) {
	ASSERT(0.0 <= pos[0] && pos[0] <= 1.0);
	ASSERT(0.0 <= pos[1] && pos[1] <= 1.0);
	Text* new_text = new Text{Vec3f{1.0f, 1.0f, 1.0f}, text, pos, scale, false};
	m_text.push_back(new_text);
	return new_text;
}

void Window::hideText(Window::TextId id) {
	id->m_hidden = true;
}

void Window::showText(Window::TextId id) {
	id->m_hidden = false;
}

bool Window::isInsideText(TextId id, Vec2f pos) {
	return 
		id->m_pos[0] <= pos[0] && pos[0] < id->m_pos[0] + textSize(id)[0] &&
		id->m_pos[1] <= pos[1] && pos[1] < id->m_pos[1] + textSize(id)[1];
}

Vec2f Window::textSize(TextId id) {
	int advance_step = 64;
	
	float width = 0;
	float height = 0;
	
	for (auto&& c : id->m_text) {
		CharacterTexture& ct = m_characters.at(c);
		width += (ct.m_advance / advance_step) * id->m_scale * S_HEIGHT / S_WIDTH;
		height = std::max(height, ct.m_bearing.y * id->m_scale);
	}
	return Vec2f{width, height};
}

void Window::setTextColor(TextId id, Vec3f color) {
	id->m_color = color;
}

void Window::setTextString(Window::TextId id, const std::string& text) {
	id->m_text = text;
}

void Window::setTextPosition(Window::TextId id, Vec2f pos) {
	id->m_pos = pos;
}

void Window::destroyText(Window::TextId id) {
	delete id;
	m_text.erase(std::find(m_text.begin(), m_text.end(), id));
}
