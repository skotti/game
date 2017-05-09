#include "shadow.h"
#include "debug.h"
#include "gl_headers.h"
const int Shadow::S_SHADOW_WIDTH = 1024;
const int Shadow::S_SHADOW_HEIGHT = 1024;

Shadow::Shadow()
{
	GL_CHECK(glGenFramebuffers(1, &depthMapFBO));
	// - Create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_SHADOW_WIDTH, S_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
