#include "shadow.h"
#include "debug.h"
#include <cmath>
#include "math.h"
#include "vector.h"
#include "window.h"
using namespace std;

Shadow::Shadow()
{
	// generate cascades sizes
	float cascade_size = Window::S_Z_FAR - Window::S_Z_NEAR;
	
	m_box_ends.at(0) = Window::S_Z_NEAR;
	for (int i = S_NUM_CASCADES; i > 0; i--) {
		m_box_ends.at(i) = Window::S_Z_NEAR + cascade_size;
		cascade_size /= 2.0f;
		std::cout<<"end : "<<m_box_ends.at(i)<<std::endl;
	}
	
// 	m_box_ends.at(1) = Window::S_Z_FAR;
	
	// bind textures
	GL_CHECK(glGenFramebuffers(1, &m_depth_fbo));
	glGenTextures(S_NUM_CASCADES, &m_depth_maps.at(0));
	for (int i = 0; i < S_NUM_CASCADES; i++) {
		glBindTexture(GL_TEXTURE_2D, m_depth_maps.at(i));

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Window::S_SHADOW_WIDTH, Window::S_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat border_color[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
	}
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_depth_fbo));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_maps.at(0), 0));
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (!(status == GL_FRAMEBUFFER_COMPLETE))
		abort();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Shadow::bindForWriting(int index)
{
	assert(index < S_NUM_CASCADES);
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_depth_fbo));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_maps.at(index), 0));
}

void Shadow::bindForReading()
{
	for (int i = 0; i < S_NUM_CASCADES; i++) {
		glActiveTexture(GL_TEXTURE1+i);
		glBindTexture(GL_TEXTURE_2D, m_depth_maps.at(i));
	}
}

std::array<glm::vec4, 8> Shadow::calcOrthoProj(int i, glm::mat4 view_camera_matrix, glm::mat4 view_light_matrix) {
	
	float reverse_z_array[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
		
	glm::mat4 reverse_z = glm::make_mat4(reverse_z_array);

	float x_y = (float)Window::S_HEIGHT/ (float)Window::S_WIDTH;
	float tan_x = std::tan(Window::S_FOV_Y / x_y / 2.0f);
  float tan_y = std::tan(Window::S_FOV_Y / 2.0f);
	
		float xn = tan_x * m_box_ends.at(i);
		float xf = tan_x * m_box_ends.at(i+1);
		float yn = tan_y * m_box_ends.at(i);
		float yf = tan_y * m_box_ends.at(i+1);

		std::array<glm::vec4, S_NUM_FRUSTUM_CORNERS> frustum_corners_camera = {
			// near face
			glm::vec4(xn, yn, m_box_ends.at(i), 1.0),
			glm::vec4(-xn, yn, m_box_ends.at(i), 1.0),
			glm::vec4(xn, -yn, m_box_ends.at(i), 1.0),
			glm::vec4(-xn, -yn, m_box_ends.at(i), 1.0),

			// far face
			glm::vec4(xf, yf, m_box_ends.at(i + 1), 1.0),
			glm::vec4(-xf, yf, m_box_ends.at(i + 1), 1.0),
			glm::vec4(xf, -yf, m_box_ends.at(i + 1), 1.0),
			glm::vec4(-xf, -yf, m_box_ends.at(i + 1), 1.0)
		};
		
		float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::min();
		float min_y = std::numeric_limits<float>::max();
		float max_y = std::numeric_limits<float>::min();
		float min_z = std::numeric_limits<float>::max();
		float max_z = std::numeric_limits<float>::min();
		
		std::array<glm::vec4, S_NUM_FRUSTUM_CORNERS> frustum_corners_world;
		for (uint j = 0 ; j < S_NUM_FRUSTUM_CORNERS; j++) {
			frustum_corners_world.at(j) = glm::inverse(view_camera_matrix) * reverse_z * frustum_corners_camera.at(j);
		}
		
		std::array<glm::vec4, S_NUM_FRUSTUM_CORNERS> frustum_corners_light;
		for (uint j = 0 ; j < S_NUM_FRUSTUM_CORNERS; j++) {
			// Transform the frustum coordinate from world to light space
			frustum_corners_light.at(j) = reverse_z * view_light_matrix * frustum_corners_world.at(j);
		}
		
		for (uint j = 0 ; j < S_NUM_FRUSTUM_CORNERS; j++) {
			min_x = std::min(min_x, frustum_corners_light.at(j).x / frustum_corners_light.at(j).w);
			max_x = std::max(max_x, frustum_corners_light.at(j).x / frustum_corners_light.at(j).w);
			min_y = std::min(min_y, frustum_corners_light.at(j).y / frustum_corners_light.at(j).w);
			max_y = std::max(max_y, frustum_corners_light.at(j).y / frustum_corners_light.at(j).w);
			min_z = std::min(min_z, frustum_corners_light.at(j).z / frustum_corners_light.at(j).w);
			max_z = std::max(max_z, frustum_corners_light.at(j).z / frustum_corners_light.at(j).w);
		}
		
		const float offset = 0.0f;
		
		m_shadow_boxes.at(i).m_right = max_x + offset;
		m_shadow_boxes.at(i).m_left = min_x - offset;
		m_shadow_boxes.at(i).m_bottom = min_y - offset;
		m_shadow_boxes.at(i).m_top = max_y + offset;
		m_shadow_boxes.at(i).m_far = max_z + offset;
		m_shadow_boxes.at(i).m_near = 0;// min_z - offset;
		
		return frustum_corners_camera;
}
