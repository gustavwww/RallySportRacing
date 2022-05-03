#include "HdrFileGenerator.h"
#include <iostream>
#include <fstream>
#include <stb_image_write.h>

namespace Utils {

	//Framebuffers
	unsigned int hdrIrraFBO;
	unsigned int mipmapFBO;

	//VertexArrayObjects
	unsigned int renderVAO;
	unsigned int renderVBO;

	unsigned int irrTexWidth = 512, irrTexHeight = 256;

	//Texture
	unsigned int HdrFileGenerator::irraTexture = 0;

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
		glViewport(0, 0, irrTexWidth, irrTexHeight );
		glClear(GL_COLOR_BUFFER_BIT);

		drawScreenQuad();
		std::vector<float> data;
		data.resize(size_t(lwidth) * lheight * 4);
		glGetTextureSubImage(irraTexture, 0, 0, 0, 0, lwidth, lheight, 1, GL_RGBA, GL_FLOAT, data.size() * sizeof(float), data.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Take data from texture and store in HDR file.
		//stbi_write_hdr("../Textures/Background/irradiance.hdr", lwidth, lheight, 4, &data[0]);

	}

	//ToDo write this function.
	void HdrFileGenerator::createReflectionHDRs(string filePath) {

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

		//Generate buffers
		glGenFramebuffers(7, &mipmapFBO);

		//Bind buffer
		glBindFramebuffer(GL_FRAMEBUFFER, mipmapFBO);
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
