#include "FrameBufferObject.h"
#include <cstdint>
#include <glm/glm.hpp>

FboInfo::FboInfo()
	: isComplete(false)
	, framebufferId(UINT32_MAX)
	, colorTextureTarget(UINT32_MAX)
	, depthBuffer(UINT32_MAX)
	, width(0)
	, height(0) {};

FboInfo::FboInfo(int w, int h) : FboInfo()
{
	resize(w, h);
};

void FboInfo::resize(int w, int h)
{
	width = w;
	height = h;

	///////////////////////////////////////////////////////////////////////
	// if no texture indices yet, allocate
	///////////////////////////////////////////////////////////////////////
	if (colorTextureTarget == UINT32_MAX)
	{
		glGenTextures(1, &colorTextureTarget);
		glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (depthBuffer == UINT32_MAX)
	{
		glGenTextures(1, &depthBuffer);
		glBindTexture(GL_TEXTURE_2D, depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glm::vec4 border(1.0f);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border.x);
	}

	///////////////////////////////////////////////////////////////////////
	// Allocate / Resize textures
	///////////////////////////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, colorTextureTarget);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
		nullptr);

	///////////////////////////////////////////////////////////////////////
	// Bind textures to framebuffer (if not already done)
	///////////////////////////////////////////////////////////////////////
	if (!isComplete)
	{
		///////////////////////////////////////////////////////////////////////
		// Generate and bind framebuffer
		///////////////////////////////////////////////////////////////////////
		glGenFramebuffers(1, &framebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

		// bind the texture as color attachment 0 (to the currently bound framebuffer)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureTarget, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		// bind the texture as depth attachment (to the currently bound framebuffer)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

		// check if framebuffer is complete
		isComplete = checkFramebufferComplete();

		// bind default framebuffer, just in case.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

bool FboInfo::checkFramebufferComplete(void)
{
	// Check that our FBO is correctly set up, this can fail if we have
	// incompatible formats in a buffer, or for example if we specify an
	// invalid drawbuffer, among things.
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		abort();
	}

	return (status == GL_FRAMEBUFFER_COMPLETE);
}
