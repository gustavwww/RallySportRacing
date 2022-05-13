#include "HdrFileGenerator.h"
#include <iostream>
#include <fstream>
#include <stb_image_write.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Utils {

	//Framebuffers
	unsigned int hdrIrraFBO;
	unsigned int hdrRefFBO;

	//VertexArrayObjects
	unsigned int renderVAO;
	unsigned int renderVBO;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	//Width and height
	unsigned int irrTexWidth = 822, irrTexHeight = 411;

	//Change depending on level.
	unsigned int refTexWidth = 410, refTexHeight = 204;

	//Texture
	unsigned int irraTexture;
	unsigned int refTexture;

	/// <param name="filePath">File path to which hdr image the irradiance hdr should be based on.</param>
	void HdrFileGenerator::createIrradianceHDR(GLuint programID, string filePath) {
		static unsigned int envTexture = 0;
		if (envTexture == 0)
		{
			envTexture = loadHDRTexture(filePath);
		}
		setUpFrameBufferIrradiance();

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
		stbi_write_hdr("../Textures/Background/irradianceTest.hdr", lwidth, lheight, 4, &data[0]);

	}

	//ToDo write this function.
	void HdrFileGenerator::createReflectionHDRs(GLuint programID, string filePath) {
		static unsigned int envTexture = 0;
		if (envTexture == 0)
		{
			envTexture = loadHDRTexture(filePath);
		}
		setUpFrameBufferReflection();

		glUseProgram(programID);

		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_2D, envTexture);

		//Change based on level.
		float roughness = 0.3;
		float max_envmap_value = 1.5f;
		glm::vec3 env_map_multiplier = glm::vec3(1.0f, 1.0f, 1.0f);

		glUniform1fv(glGetUniformLocation(programID, "roughness"), 1, &roughness);
		glUniform1fv(glGetUniformLocation(programID, "max_envmap_value"), 1, &max_envmap_value);
		glUniform3fv(glGetUniformLocation(programID, "env_map_multiplier"), 1, &env_map_multiplier[0]);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrRefFBO);

		GLint lwidth, lheight;
		glGetTextureLevelParameteriv(refTexture, 0, GL_TEXTURE_WIDTH, &lwidth);
		glGetTextureLevelParameteriv(refTexture, 0, GL_TEXTURE_HEIGHT, &lheight);
		glViewport(0, 0, refTexWidth, refTexHeight);
		glClear(GL_COLOR_BUFFER_BIT);

		drawScreenQuad();
		std::vector<float> data;
		data.resize(size_t(lwidth) * lheight * 4);
		glGetTextureSubImage(refTexture, 0, 0, 0, 0, lwidth, lheight, 1, GL_RGBA, GL_FLOAT, data.size() * sizeof(float), data.data());

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
	void HdrFileGenerator::setUpFrameBufferIrradiance() {

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
	void HdrFileGenerator::setUpFrameBufferReflection() {

		if (hdrRefFBO != 0)
		{
			return;
		}
		//Generate buffer.
		glGenFramebuffers(1, &hdrRefFBO);

		//Bind buffer.
		glBindFramebuffer(GL_FRAMEBUFFER, hdrRefFBO);

		glGenTextures(1, &refTexture);
		glBindTexture(GL_TEXTURE_2D, refTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, refTexWidth, refTexHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refTexture, 0);
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
}