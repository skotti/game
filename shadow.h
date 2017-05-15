#ifndef SHADOW_H
#define SHADOW_H

#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <vector>

struct ShadowBox {
	float m_right;
	float m_left;
	float m_bottom;
	float m_top;
	float m_far;
	float m_near;
};

class Shadow {

public: 
	
	Shadow ();
	
	GLuint getFBO() {
		return m_depth_fbo;
	}
	
	GLuint getMap(int i) {
		return m_depth_maps.at(i);
	}
	
	GLfloat getEnd(int i) {
		return m_box_ends.at(i);
	}
	
	ShadowBox getBox(int i) {
		return m_shadow_boxes.at(i);
	}
	
	void bindForWriting(int index);
	void bindForReading();
	
	void calcOrthoProj(int index, glm::mat4 view_camera_matrix, glm::mat4 view_light_matrix);
	
	static constexpr int S_NUM_CASCADES = 3;
	static constexpr int S_NUM_FRUSTUM_CORNERS = 8;
private :
	
	GLuint m_depth_fbo;
	
	std::array<GLuint, S_NUM_CASCADES> m_depth_maps;
	std::array<ShadowBox, S_NUM_CASCADES> m_shadow_boxes;
	std::array<GLfloat, S_NUM_CASCADES+1> m_box_ends;
};

#endif