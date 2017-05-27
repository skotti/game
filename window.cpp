#include "window.h"
#include "debug.h"

#include "logger.h"
#include "gl_headers.h"

const GLuint Window::S_WIDTH = 1200;
const GLuint Window::S_HEIGHT = 900; 

const float Window::S_Z_NEAR = 0.1;
const float Window::S_Z_FAR = 10.0; 
const float Window::S_FOV_Y = 45.0 / 180.0 * M_PI;//45.0;

const GLuint Window::S_SHADOW_WIDTH = 1920;
const GLuint Window::S_SHADOW_HEIGHT = 1080;

std::map<std::string, VisualObject> Window::S_VISUAL_OBJECT;

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
	createShadowShader();
	createSkyBoxShader();
	createInterShader();
	
	// create light source
// 	Vec3f light_direction = {1.0, -1.0, 1.0};
// 	Vec3f light_ambient = {0.1f, 0.1f, 0.1f};
// 	Vec3f light_diffuse = {0.8f, 0.8f, 0.8f};
// 	Vec3f light_specular = {0.5, 0.5, 0.5};
// 	int light_type = 1;
	
// 	m_light = Ligsdht(light_direction, light_ambient, light_specular, light_diffuse, light_type);
//	m_skybox = new SkyBox();
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
	Shader vertex_shader("shaders/cascade_depth.vert", ShaderType::VERTEX);
	Shader fragment_shader("shaders/cascade_depth.frag", ShaderType::FRAGMENT);
	
	m_depth_shader->attach(&vertex_shader);
	m_depth_shader->attach(&fragment_shader);
	m_depth_shader->compile();
	
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
	
	glm::vec3 camera_pos = toGLMvec3(getCamera().getPos());
	glm::vec3 camera_front = toGLMvec3(getCamera().getFront());
	glm::vec3 camera_up = toGLMvec3(getCamera().getUp());
	
	
	GLuint program = m_depth_shader->getShaderProgram();
	GL_CHECK(glUseProgram(program)); 
	glViewport(0, 0, Window::S_SHADOW_WIDTH, Window::S_SHADOW_HEIGHT); 
	//renderShadows
	std::array<glm::mat4, Shadow::S_NUM_CASCADES> light_projection_matrix;
	std::array<glm::mat4, Shadow::S_NUM_CASCADES> light_vp_matrix; 
	glm::vec3 light_pos = glm::vec3(-2.0f, 2.0f, -2.0f);
 	glm::vec3 light_dir = glm::vec3(1.0f, -1.0f, 1.0f);

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
			VisualObject* vo = view->m_visual_object;
			
			GL_CHECK(glBindVertexArray(vo->m_model->getVAO()));

			glm::mat4 model;
			model = glm::translate(model, toGLMvec3(view->m_pos));
			model = glm::scale(model, toGLMvec3(view->m_size));
			glm::mat4 mvp = light_vp_matrix.at(i) * model;

			GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp)));
			GL_CHECK(glDrawElements(GL_TRIANGLES, vo->m_model->getIndicesSize(), GL_UNSIGNED_INT, 0));

			GL_CHECK(glBindVertexArray(0));
		}
	//-----------------------------------------------------------------------
	}
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glViewport(0, 0, S_WIDTH, S_HEIGHT);  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "shadow_maps[0]"), m_shadow->getMap(0)));
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "shadow_maps[1]"), m_shadow->getMap(1)));
	GL_CHECK(glUniform1i(glGetUniformLocation(program, "shadow_maps[2]"), m_shadow->getMap(2)));
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
	//render RenderScene
	for (int j = 0; j < m_view.size(); j++) {

		if (m_view.at(j) == nullptr) continue;
		
		View* view = m_view.at(j);
		VisualObject* vo = view->m_visual_object;
		
		GL_CHECK(glBindVertexArray(vo->m_model->getVAO()));

		glm::mat4 model;
		model = glm::translate(model, toGLMvec3(view->m_pos));
		model = glm::scale(model, toGLMvec3(view->m_size));

		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix)));
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model)));
		GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
		GL_CHECK(glDrawElements(GL_TRIANGLES, vo->m_model->getIndicesSize(), GL_UNSIGNED_INT, 0));

		GL_CHECK(glBindVertexArray(0));
	}
		
}