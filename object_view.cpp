#include "object_view.h"
#include "gl_headers.h"

void ObjectView::draw(Window* window, Vec3f pos, Vec3f angle, Vec3f size)
{
	
	int width, height;
	glfwGetFramebufferSize(window->getGLWindow(), &width, &height);  
	
	glm::vec3 cameraPos = window->getCamera().getPos();
	glm::vec3 cameraFront = window->getCamera().getFront();
	glm::vec3 cameraUp = window->getCamera().getUp();
	
	GLuint program = m_shader_program->getShaderProgram();
	
	GL_CHECK(glUseProgram(program)); 
	
	if (hasTexture()) {
		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture->getGLid()));
		GL_CHECK(glUniform1i(glGetUniformLocation(program, "tex"), 0));
		
	} else if (hasMaterial()) {
		
		GLint viewPosLoc = GL_CHECK(glGetUniformLocation(program, "viewPos"));
		GL_CHECK(glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z));
		// Set lights properties
		glm::vec3 lightColor;
		lightColor.x = 1.0f;
		lightColor.y = 1.0f;
		lightColor.z = 1.0f;
		//ТЕПЕРЬ ЦИКЛ ПО ВСЕМ ИСТОЧНИКАМ
		Light* dirLight = m_lights.at(0);
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.direction"), dirLight->m_direction[0], dirLight->m_direction[1], dirLight->m_direction[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.ambient"), dirLight->m_ambient[0], dirLight->m_ambient[1], dirLight->m_ambient[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.diffuse"), dirLight->m_diffuse[0], dirLight->m_diffuse[1], dirLight->m_diffuse[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "dirLight.specular"), dirLight->m_specular[0], dirLight->m_specular[1], dirLight->m_specular[2]));
		
		
		std::string cur_string;
		for (int i = 0; i < 4; i++) {
			cur_string = "pointLights["+std::to_string(i)+"].position";
			GL_CHECK(glUniform3f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_position[0], m_lights.at(i+1)->m_position[1], m_lights.at(i+1)->m_position[2]));
			cur_string = "pointLights["+std::to_string(i)+"].ambient";
			GL_CHECK(glUniform3f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_ambient[0], m_lights.at(i+1)->m_ambient[1], m_lights.at(i+1)->m_ambient[2]));
			cur_string = "pointLights["+std::to_string(i)+"].diffuse";
			GL_CHECK(glUniform3f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_diffuse[0], m_lights.at(i+1)->m_diffuse[1], m_lights.at(i+1)->m_diffuse[2]));
			cur_string = "pointLights["+std::to_string(i)+"].specular";
			GL_CHECK(glUniform3f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_specular[0], m_lights.at(i+1)->m_specular[1], m_lights.at(i+1)->m_specular[2]));
			cur_string = "pointLights["+std::to_string(i)+"].constant";
			GL_CHECK(glUniform1f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_constant));
			cur_string = "pointLights["+std::to_string(i)+"].linear";
			GL_CHECK(glUniform1f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_linear));
			cur_string = "pointLights["+std::to_string(i)+"].quadratic";
			GL_CHECK(glUniform1f(glGetUniformLocation(program, cur_string.c_str()), m_lights.at(i+1)->m_quadratic));
		}
		// Set material properties
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.ambient"),   m_material->m_ambient[0], m_material->m_ambient[1], m_material->m_ambient[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.diffuse"),   m_material->m_diffuse[0], m_material->m_diffuse[1], m_material->m_diffuse[2]));
		GL_CHECK(glUniform3f(glGetUniformLocation(program, "material.specular"),  m_material->m_specular[0], m_material->m_specular[1], m_material->m_specular[2])); 
		GL_CHECK(glUniform1f(glGetUniformLocation(program, "material.shininess"), m_material->m_shine));
	}
	glm::mat4 view;
	GLint viewLoc = GL_CHECK(glGetUniformLocation(program, "view"));
	view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
	GL_CHECK(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
	
	
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	GLint projLoc = GL_CHECK(glGetUniformLocation(program, "projection"));
	GL_CHECK(glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)));
	
	GL_CHECK(glBindVertexArray(m_model->getVAO()));
	
	GLint modelLoc = GL_CHECK(glGetUniformLocation(program, "model"));
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(size[0], size[1], size[2]));
	model = glm::translate(model, glm::vec3(pos[0], pos[1], pos[2]));
	model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.3f, 0.5f));
	GL_CHECK(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	GL_CHECK(glDrawElements(GL_TRIANGLES, m_model->getIndicesSize(), GL_UNSIGNED_INT, 0));
	
	GL_CHECK(glBindVertexArray(0));
}
