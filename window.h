#ifndef WINDOW_H
#define WINDOW_H
#include "gl_headers.h"
#include "logger.h"
#include "debug.h"
#include "camera.h"
#include "material.h"
#include "yaml-cpp/yaml.h"
#include "logger.h"
#include "singleton.h"
#include "vector.h"
#include "model.h"
#include "shader.h"
#include "shader_program.h"
#include "lights.h"

class GameObject;

struct VisualObject {
	
	Model* m_model;
	Material* m_material;
};


struct View {
	
	VisualObject* m_visual_object = nullptr;
	Vec3f m_pos = Vec3f{0, 0, 0};
	Vec3f m_size = Vec3f{0, 0, 0};
	Vec3f m_front = Vec3f{0, 0, 0};
	Vec3f m_up = Vec3f{0, 0, 0};
};

class Window : public Singleton<Window> {
	
public:
	Window();
	~Window();
	GLFWwindow* getGLWindow() {
		return m_window;
	}
	void clearScreen() {
		GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	
	void swapBuffers() {
		glfwSwapBuffers(m_window);
	}
	
	Camera& getCamera() { return m_camera; }
	
	void setPos(int m_id, Vec3f pos) {
		ASSERT(m_view.at(m_id) != nullptr);
		m_view.at(m_id)->m_pos = pos;
	}
	
	void setFront(int m_id, Vec3f front) {
		ASSERT(m_view.at(m_id) != nullptr);
		m_view.at(m_id)->m_front = front;
	}
	
	void setUp(int m_id, Vec3f up) {
		ASSERT(m_view.at(m_id) != nullptr);
		m_view.at(m_id)->m_up = up;
	}
	
	void setSize(int m_id, Vec3f size) {
		ASSERT(m_view.at(m_id) != nullptr);
		m_view.at(m_id)->m_size = size;
	}
	
	void draw();

	int registerGameObject(const std::string& window_model_name);
	void destroyGameObject(int m_id);
	
	static const float S_Z_NEAR;
	
private:
	
	void createObjectShader();
	void createDepthShader();
	
	void createVisualObjects();
	
	GLFWwindow* m_window;
	Logger m_logger;
	Camera m_camera;
	
	int objectIdCounter = 0;
	static std::map<std::string, VisualObject> S_VISUAL_OBJECT;
	
	std::vector<View*> m_view;

	static const float S_Z_FAR;
	static const float S_FOV_Y;
	
	static const GLuint S_WIDTH, S_HEIGHT; 
	
	Light m_light;
	ShaderProgram* m_object_shader = nullptr;
	ShaderProgram* m_depth_shader = nullptr;
};

#endif