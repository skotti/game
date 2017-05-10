#include "window.h"
#include "debug.h"

#include "logger.h"
#include "gl_headers.h"
#include "stl_headers.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

const GLuint Window::S_WIDTH = 1280;
const GLuint Window::S_HEIGHT = 960;

const float Window::S_Z_NEAR = 0.1;
const float Window::S_Z_FAR = 100; 
const float Window::S_FOV_Y = 45.0;

std::map<std::string, VisualObject> Window::S_VISUAL_OBJECT;

Window::Window() {
	m_logger.log("Create window");
	
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	m_window = glfwCreateWindow(S_WIDTH, S_HEIGHT, "Cube Game", nullptr, nullptr);
	ASSERT(m_window != nullptr);
	glfwMakeContextCurrent(m_window);
	
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	while(glGetError()) {} // eat error produced by glew

	GL_CHECK(glViewport(0, 0, S_WIDTH, S_HEIGHT));
	
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthFunc(GL_LESS));
	
	// to draw fonts
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	createVisualObjects();
	
	createObjectShader();
	createDepthShader();
	createFontShader();
	
	// create light source
	Vec3f light_direction = {-0.2, -1.0, -0.3};
	Vec3f light_ambient = {0.633, 0.727811, 0.633};
	Vec3f light_diffuse = {0.7, 0.7, 0.7};
	Vec3f light_specular = {0.5, 0.5, 0.5};
	int light_type = 1;
	
	m_light = Light(light_direction, light_ambient, light_specular, light_diffuse, light_type);
	
	initFonts();
	initTextObject();
}

Window::~Window() {
	ASSERT(m_object_shader != nullptr);
	delete m_object_shader;
	ASSERT(m_depth_shader != nullptr)
	delete m_depth_shader;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


int Window::registerGameObject(const std::string& window_model_name) {
	View* view = new View();
	view->m_visual_object = &S_VISUAL_OBJECT.at(window_model_name);
	m_view.push_back(view);
	int id = objectIdCounter++;
	return id;
}

void Window::destroyGameObject(int id)
{
	ASSERT(m_view.at(id) != nullptr);
	delete m_view.at(id);
}

void Window::createVisualObjects() {
	
	Model* model = new Model("models/modelCube.obj");
	Material* material = new Material(
		Vec3f{0.0215, 0.1745, 0.0215},
		Vec3f{0.633, 0.727811, 0.633},
		Vec3f{0.07568, 0.61424, 0.07568},
		1);
	
	S_VISUAL_OBJECT["cube"] = VisualObject{model, material};
	
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
	Shader vertex_shader("shaders/depth_vertex.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/depth_fragment.frag", ShaderType::FRAGMENT);
	
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

void Window::draw() {	
	glm::vec3 camera_pos = toGLMvec3(getCamera().getPos());
	glm::vec3 camera_front = toGLMvec3(getCamera().getFront());
	glm::vec3 camera_up = toGLMvec3(getCamera().getUp());
	
	GLuint program = m_object_shader->getShaderProgram();
	
	GL_CHECK(glUseProgram(program));  
	
	GLint view_pos_loc = GL_CHECK(glGetUniformLocation(program, "viewPos"));
	GL_CHECK(glUniform3f(view_pos_loc, camera_pos.x, camera_pos.y, camera_pos.z));
		
	for (int i = 0; i < m_view.size(); i++) {
		
		if (m_view.at(i) == nullptr) continue;
		
		View* view = m_view.at(i);
		VisualObject* vo = view->m_visual_object;
		Material* mat = vo->m_material;
		
		int start_of_point_sources;
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.direction"), m_light.m_direction[0], m_light.m_direction[1], m_light.m_direction[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.ambient"), m_light.m_ambient[0], m_light.m_ambient[1], m_light.m_ambient[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.diffuse"), m_light.m_diffuse[0], m_light.m_diffuse[1], m_light.m_diffuse[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.specular"), m_light.m_specular[0], m_light.m_specular[1], m_light.m_specular[2]));

		// Set material properties
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.ambient"), mat->m_ambient[0], mat->m_ambient[1], mat->m_ambient[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.diffuse"), mat->m_diffuse[0], mat->m_diffuse[1], mat->m_diffuse[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.specular"), mat->m_specular[0], mat->m_specular[1], mat->m_specular[2])); 
		GL_CHECK(glUniform1f(glGetUniformLocation(program, "material.shininess"), mat->m_shine));
		
		glm::mat4 view_matrix;
		GLint view_loc = GL_CHECK(glGetUniformLocation(program, "view"));
		view_matrix = glm::lookAt(camera_pos, camera_front + camera_pos, camera_up);
		GL_CHECK(glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix)));
		
		glm::mat4 projection;
		projection = glm::perspective(S_FOV_Y, (GLfloat)S_WIDTH / (GLfloat)S_HEIGHT, S_Z_NEAR, S_Z_FAR);
		GLint proj_loc = GL_CHECK(glGetUniformLocation(program, "projection"));
		GL_CHECK(glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection)));
		
		GL_CHECK(glBindVertexArray(vo->m_model->getVAO()));
		
		GLint model_loc = GL_CHECK(glGetUniformLocation(program, "model"));
		glm::mat4 model;
		model = glm::translate(model, toGLMvec3(view->m_pos));
		model = glm::scale(model, toGLMvec3(view->m_size));
		
		GL_CHECK(glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model)));
		GL_CHECK(glDrawElements(GL_TRIANGLES, vo->m_model->getIndicesSize(), GL_UNSIGNED_INT, 0));
		
		GL_CHECK(glBindVertexArray(0));
	}
	
	drawFonts();
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
	
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	
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
	
	GL_CHECK(glUseProgram(m_font_shader->getShaderProgram()));
	loc = GL_CHECK(glGetUniformLocation(m_font_shader->getShaderProgram(), "textColor"));
	GL_CHECK(glUniform3f(loc, 25.0f, 25.0f, 1.0f));
	
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindVertexArray(m_text_object.m_vao));
	
	for (auto&& text : m_text) {
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
	Text* new_text = new Text{text, pos, scale};
	m_text.push_back(new_text);
	return new_text;
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