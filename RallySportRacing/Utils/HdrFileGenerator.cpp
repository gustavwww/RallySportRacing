#include "HdrFileGenerator.h"
#include <iostream>
#include <fstream>
#include <stb_image_write.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Utils {

	//Framebuffers
	unsigned int hdrIrraFBO;
	unsigned int mipmapFBO;
	unsigned int shadowMapFBO;

	//VertexArrayObjects
	unsigned int renderVAO;
	unsigned int renderVBO;

	//Width and height
	unsigned int irrTexWidth = 822, irrTexHeight = 411;

	//Texture
	unsigned int irraTexture;
	unsigned int texture;
	unsigned int shadowMap;

	/// <param name="filePath">File path to which hdr image the irradiance hdr should be based on.</param>
	void HdrFileGenerator::createIrradianceHDR(GLuint programID, string filePath) {
		static unsigned int envTexture = 0;
		if ( envTexture == 0 )
		{
			envTexture = loadHDRTexture( filePath );
		}
		setUpFrameBuffer();

		glUseProgram(programID);

		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_2D, envTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrIrraFBO);

		GLint lwidth, lheight;
		glGetTextureLevelParameteriv(irraTexture, 0, GL_TEXTURE_WIDTH, &lwidth);
		glGetTextureLevelParameteriv(irraTexture, 0, GL_TEXTURE_HEIGHT, &lheight);
		glViewport(0, 0, irrTexWidth, irrTexHeight);
		glClear(GL_COLOR_BUFFER_BIT);

		drawScreenQuad();
		std::vector<float> data;
		data.resize(size_t(lwidth) * lheight * 4);
		glGetTextureSubImage(irraTexture, 0, 0, 0, 0, lwidth, lheight, 1, GL_RGBA, GL_FLOAT, data.size() * sizeof(float), data.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Take data from texture and store in HDR file.
		stbi_flip_vertically_on_write(1);
		stbi_write_hdr("../Textures/Background/irradiance.hdr", lwidth, lheight, 4, &data[0]);

	}

	//ToDo write this function.
	void HdrFileGenerator::createReflectionHDRs(GLuint programID, string filePath) {
		static unsigned int envTexture = 0;
		if (envTexture == 0)
		{
			envTexture = loadHDRTexture(filePath);
		}
		setUpFrameBuffers();

		glUseProgram(programID);

		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_2D, envTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, mipmapFBO);

		GLint lwidth, lheight;
		glGetTextureLevelParameteriv(mipmapFBO, 1, GL_TEXTURE_WIDTH, &lwidth);
		glGetTextureLevelParameteriv(mipmapFBO, 1, GL_TEXTURE_HEIGHT, &lheight);
		glViewport(0, 0, 1643, 821);
		glClear(GL_COLOR_BUFFER_BIT);

		drawScreenQuad();
		std::vector<float> data;
		data.resize(size_t(lwidth) * lheight * 4);
		glGetTextureSubImage(irraTexture, 1, 0, 0, 0, lwidth, lheight, 1, GL_RGBA, GL_FLOAT, data.size() * sizeof(float), data.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Take data from texture and store in HDR file.
		stbi_flip_vertically_on_write(1);
		stbi_write_hdr("../Textures/Background/test.hdr", lwidth, lheight, 4, &data[0]);
	}

	unsigned int HdrFileGenerator::loadHDRTexture(const std::string& filename) {

		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		HDRImage image(filename);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image.width, image.height, 0, GL_RGB, GL_FLOAT, image.data);

		return textureID;
	}

	//ToDo test
	unsigned int HdrFileGenerator::loadHdrMipmapTexture(const std::vector<std::string>& filenames) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		//ToDo maybe chabge this.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		for (int i = 0; i < filenames.size(); i++)
		{
			HDRImage image(filenames[i]);
			glTexImage2D(GL_TEXTURE_2D, i, GL_RGB32F, image.width, image.height, 0, GL_RGB, GL_FLOAT, image.data);
			if (i == 0)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}

		return textureID;
	}

	//ToDo make sure this works.
	void HdrFileGenerator::setUpFrameBuffer() {

		if ( hdrIrraFBO != 0 )
		{
			return;
		}
		//Generate buffer.
		glGenFramebuffers(1, &hdrIrraFBO);

		//Bind buffer.
		glBindFramebuffer(GL_FRAMEBUFFER, hdrIrraFBO);
	
		glGenTextures(1, &irraTexture);
		glBindTexture(GL_TEXTURE_2D, irraTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, irrTexWidth, irrTexHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, irraTexture, 0);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers( 1, attachments );

		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if ( status != GL_FRAMEBUFFER_COMPLETE )
		{
			throw "Framebuffer not complete";
		}
	}

	//ToDo finish writing this function and change width and height.
	void HdrFileGenerator::setUpFrameBuffers() {

		if (mipmapFBO != 0)
		{
			return;
		}
		//Generate buffer.
		glGenFramebuffers(1, &mipmapFBO);

		//Bind buffer.
		glBindFramebuffer(GL_FRAMEBUFFER, mipmapFBO);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1643, 821, 0, GL_RGBA, GL_FLOAT, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			throw "Framebuffer not complete";
		}
	}

	void HdrFileGenerator::drawScreenQuad() {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		if (renderVAO == 0) {
			//Generate
			glGenVertexArrays(1, &renderVAO);
			glGenBuffers(1, &renderVBO);
			//Bind
			glBindVertexArray(renderVAO);
			glBindBuffer(GL_ARRAY_BUFFER, renderVBO);
			//Set up vertices data
			static const glm::vec2 positions[] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f },
												   { -1.0f, -1.0f }, { 1.0f, 1.0f },  { -1.0f, 1.0f } };
			glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		}
		//Render
		glBindVertexArray(renderVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

	}

	void HdrFileGenerator::setUpshadowMapFrameBuffer() {

		if (shadowMapFBO != 0)
		{
			return;
		}

		//Generate depthBuffer.
		glGenFramebuffers(1, &shadowMapFBO);

		//Generate depthMapTexture.
		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		//Setup for framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
		
		//Prevent from reading or writing to the color buffer.
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		//Set default framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void HdrFileGenerator::createShadowMap(GLuint programID, glm::vec3 lightPos) {

		setUpshadowMapFrameBuffer();

		//ToDo mess around with these numbers until the shadows look good.
		float nearPlane = 1.0f, farPlane = 75.0f;
		glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glUseProgram(programID);
		glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//ToDo render scene here.


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//ToDo finish method.
	void HdrFileGenerator::renderShadowMap(GLuint programID) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1024, 1024);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);

	}
}
