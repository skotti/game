#include "window.h"
#include "debug.h"

#include "logger.h"
#include "gl_headers.h"

const GLuint Window::S_WIDTH = 1024;
const GLuint Window::S_HEIGHT = 768; 

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
	
	createVisualObjects();
	
	createObjectShader();
	createDepthShader();
	
	// create light source
	Vec3f light_direction = {-0.2, -1.0, -0.3};
	Vec3f light_ambient = {0.633, 0.727811, 0.633};
	Vec3f light_diffuse = {0.7, 0.7, 0.7};
	Vec3f light_specular = {0.5, 0.5, 0.5};
	int light_type = 1;
	
	m_light = Light(light_direction, light_ambient, light_specular, light_diffuse, light_type);
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

void Window::destroyGameObject(int m_id)
{
	ASSERT(m_view.at(m_id) != nullptr);
	delete m_view.at(m_id);
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
}