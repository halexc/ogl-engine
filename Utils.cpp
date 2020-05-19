#include "Utils.h"

void Utils::LoadTextureFromImage(GLuint * inTexID, const char * path)
{
	glGenTextures(1, inTexID);
	*inTexID = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, *inTexID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
}

bool Utils::GenerateDepthFBO(GLuint * fb, GLuint * texId, GLsizei width, GLsizei height)
{
	GLint framebufferOld;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferOld);

	// Create Framebuffer Object.
	glGenFramebuffers(1, fb);
	glBindFramebuffer(GL_FRAMEBUFFER, *fb);

	// Create Texture.
	glGenTextures(1, texId);
	glBindTexture(GL_TEXTURE_2D, *texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	// Configure Texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferOld);
	return true;
}
