#include "window.h"
#include "debug.h"

#include "logger.h"
#include "gl_headers.h"

const GLuint Window::S_WIDTH = 800;
const GLuint Window::S_HEIGHT = 600; 

const float Window::S_Z_NEAR = 1.0;
const float Window::S_Z_FAR = 100; 
const float Window::S_FOV_Y = 45.0;

const GLuint Window::S_SHADOW_WIDTH = 1920;
const GLuint Window::S_SHADOW_HEIGHT = 1200;

std::map<std::string, VisualObject> Window::S_VISUAL_OBJECT;

Window::Window() {
	m_logger.log("Create window");
	
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
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
	
	glEnable(GL_MULTISAMPLE);  
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthFunc(GL_LESS));
	
	createVisualObjects();
	
	createObjectShader();
	createDepthShader();
	createSkyBoxShader();
	
	// create light source
	Vec3f light_direction = {-0.2, -1.0, -0.3};
	Vec3f light_ambient = {0.1f, 0.1f, 0.1f};
	Vec3f light_diffuse = {0.8f, 0.8f, 0.8f};
	Vec3f light_specular = {0.5, 0.5, 0.5};
	int light_type = 1;
	
	m_light = Light(light_direction, light_ambient, light_specular, light_diffuse, light_type);
	m_skybox = new SkyBox();
	m_shadow = new Shadow();
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
		0.6);
	
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

void Window::createSkyBoxShader() {
	m_skybox_shader = new ShaderProgram();
	Shader vertex_shader("shaders/skybox_vertex.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/skybox_fragment.frag", ShaderType::FRAGMENT);
	
	m_skybox_shader->attach(&vertex_shader);
	m_skybox_shader->attach(&fragment_shader);
	m_skybox_shader->compile();
	
}

void Window::draw() {
	
	glm::vec3 camera_pos = toGLMvec3(getCamera().getPos());
	glm::vec3 camera_front = toGLMvec3(getCamera().getFront());
	glm::vec3 camera_up = toGLMvec3(getCamera().getUp());
	glm::vec4 light_projection_matrix[Shadow::S_NUM_CASCADES];
	glm::vec4 light_space_matrix[Shadow::S_NUM_CASCADES];
	
	//set program-------------------------------------------------------------
	GLuint program = m_depth_shader->getShaderProgram();
	GL_CHECK(glUseProgram(program));
	//------------------------------------------------------------------------
	
	//set screen--------------------------------------------------------------
	glViewport(0, 0, S_SHADOW_WIDTH, S_SHADOW_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT);	
	glCullFace(GL_FRONT);
	//------------------------------------------------------------------------
	
	//set position and direction of light-------------------------------------
	glm::vec3 pos = glm::vec3(-2.0f, 4.0f, -2.0f);
	glm::vec3 direction = glm::vec3{1.0, -1.0, 1.0};
	//------------------------------------------------------------------------
	
	//renderShadows
	for (int i = 0; i < Shadow::S_NUM_CASCADES; i++) {
		glm::mat4 light_projection_matrix;
		glm::mat4 light_space_matrix;
		
		//bind current cascade--------------------------------------------------
		m_shadow->bindforwriting(i);
		//----------------------------------------------------------------------
		
		//set view camera and light matrix--------------------------------------
		glm::mat4 view_camera_matrix = glm::lookAt(camera_pos, camera_front + camera_pos, camera_up);
		glm::mat4 view_light_matrix = glm::lookAt(pos, pos + direction, glm::vec3(0.0, 1.0, 0.0));
		//----------------------------------------------------------------------
		
		//calc box coordinates for current cascade------------------------------
		ShadowBox shadow_info = m_shadow->calcOrthoProj(i, view_light_matrix, view_camera_matrix);
		//----------------------------------------------------------------------
		
		//set light projection matrix and light space matrix based on this box--
		light_projection_matrix[i] = glm::ortho(shadow_info.left, shadow_info.right, shadow_info.bottom, shadow_info.top, shadow_info.near, shadow_info.far);
		light_space_matrix[i] = light_projection_matrix[i] * view_light_matrix;
		//----------------------------------------------------------------------
		
		//render current scene in this box--------------------------------------
		for (int i = 0; i < m_view.size(); i++) {
			
			if (m_view.at(i) == nullptr) continue;
			View* view = m_view.at(i);
			VisualObject* vo = view->m_visual_object;
			GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(light_space_matrix[i])));
			renderScene(vo, view, program);
		}
		//-----------------------------------------------------------------------
	}
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	
	
	//set the main program-----------------------------------------------------
	program = m_object_shader->getShaderProgram();
	GL_CHECK(glUseProgram(program));  
	
	//set current view position------------------------------------------------
	GLint view_pos_loc = GL_CHECK(glGetUniformLocation(program, "viewPos"));
	GL_CHECK(glUniform3f(view_pos_loc, camera_pos.x, camera_pos.y, camera_pos.z)); 
	//-------------------------------------------------------------------------
	
	
	for (int i = 0; i < m_view.size(); i++) {
		
		if (m_view.at(i) == nullptr) continue;
		
		View* view = m_view.at(i);
		VisualObject* vo = view->m_visual_object;
		Material* mat = vo->m_material;
		
		//set light properties----------------------------------------------------
		int start_of_point_sources;
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.direction"), direction.x, direction.y, direction.z);/*m_light.m_direction[0], m_light.m_direction[1], m_light.m_direction[2])*/);
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.ambient"), m_light.m_ambient[0], m_light.m_ambient[1], m_light.m_ambient[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.diffuse"), m_light.m_diffuse[0], m_light.m_diffuse[1], m_light.m_diffuse[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.specular"), m_light.m_specular[0], m_light.m_specular[1], m_light.m_specular[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "lightPos"), pos.x, pos.y, pos.z));
		//------------------------------------------------------------------------
		
		//set all matricies-------------------------------------------------------
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[0]"), 1, GL_FALSE, glm::value_ptr(light_space_matrix[0])));
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[1]"), 1, GL_FALSE, glm::value_ptr(light_space_matrix[1])));
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[2]"), 1, GL_FALSE, glm::value_ptr(light_space_matrix[2])));
		//------------------------------------------------------------------------
		
		//set shadow maps---------------------------------------------------------
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "shadowMaps[0]"), 1, GL_FALSE, glm::value_ptr(m_shadow->getMap(0))));
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "shadowMaps[1]"), 1, GL_FALSE, glm::value_ptr(m_shadow->getMap(1))));
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "shadowMaps[2]"), 1, GL_FALSE, glm::value_ptr(m_shadow->getMap(2))));
		//------------------------------------------------------------------------
		
		glm::mat4 view_matrix;
		GLint view_loc = GL_CHECK(glGetUniformLocation(program, "view"));
		view_matrix = glm::lookAt(camera_pos, camera_front + camera_pos, camera_up);
		GL_CHECK(glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix)));
		
		glm::mat4 projection;
		projection = glm::perspective(S_FOV_Y, (GLfloat)S_WIDTH / (GLfloat)S_HEIGHT, S_Z_NEAR, S_Z_FAR);
		GLint proj_loc = GL_CHECK(glGetUniformLocation(program, "projection"));
		GL_CHECK(glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection)));
		
		//set cascade ends--------------------------------------------------------
		glm::vec4 end = projection * glm::vec4(0.0, 0.0, m_shadow->getEnd(0), 1.0);
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "cascadeEnds[0]"), 1, GL_FALSE, glm::value_ptr(end.z)));
		end = projection * glm::vec4(0.0, 0.0, m_shadow->getEnd(1), 1.0);
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "cascadeEnds[1]"), 1, GL_FALSE, glm::value_ptr(end.z)));
		end = projection * glm::vec4(0.0, 0.0, m_shadow->getEnd(2), 1.0);
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "cascadeEnds[2]"), 1, GL_FALSE, glm::value_ptr(end.z)));
		//------------------------------------------------------------------------
		
		glActiveTexture(GL_TEXTURE1);
    m_shadow->bindforreading();
		renderScene(vo, view, program);
	}
}

void Window::renderScene(VisualObject* vo, View* view, GLuint program) {
	GL_CHECK(glBindVertexArray(vo->m_model->getVAO()));

	GLint model_loc = GL_CHECK(glGetUniformLocation(program, "model"));
	glm::mat4 model;
	model = glm::translate(model, toGLMvec3(view->m_pos));
	model = glm::scale(model, toGLMvec3(view->m_size));

	GL_CHECK(glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model)));
	GL_CHECK(glDrawElements(GL_TRIANGLES, vo->m_model->getIndicesSize(), GL_UNSIGNED_INT, 0));

	GL_CHECK(glBindVertexArray(0));
}