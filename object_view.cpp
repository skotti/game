#include "object_view.h"
#include "gl_headers.h"

void ObjectView::draw(Window* window, Vec3f pos, Vec3f angle)
{
	
	int width, height;
	glfwGetFramebufferSize(window->getGLWindow(), &width, &height);  
	
	glm::vec3 cameraPos = window->getCamera().getPos();
	glm::vec3 cameraFront = window->getCamera().getFront();
	glm::vec3 cameraUp = window->getCamera().getUp();
	
	GLuint program = m_shader_program->getShaderProgram();
	
	GL_CHECK(glUseProgram(program));
	
	
	glm::mat4 view;
	GLint viewLoc = glGetUniformLocation(program, "view");
	view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
	GL_CHECK(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
	
	
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	GLint projLoc = GL_CHECK(glGetUniformLocation(program, "projection"));
	GL_CHECK(glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)));
	
	GL_CHECK(glBindVertexArray(m_model->getVAO()));
	
	GLint modelLoc = GL_CHECK(glGetUniformLocation(program, "model"));
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(pos[0], pos[1], pos[2]));
	model = glm::rotate(model,70.0f, glm::vec3(1.0f, 0.3f, 0.5f));
	GL_CHECK(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	GL_CHECK(glDrawElements(GL_TRIANGLES, m_model->getIndicesSize(), GL_UNSIGNED_INT, 0));
	
	GL_CHECK(glBindVertexArray(0));
}
