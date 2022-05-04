#include "Text2D.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <GL/glew.h>

using namespace std;

namespace Rendering {


	Text2D::Text2D(string text) {
		this->text = text;
		this->color = color;
	}


	void Text2D::setupText() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VAO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


	void Text2D::render(GLint programID) {
		if (!isTextSetup) {
			setupText();
			isTextSetup = true;
		}

		GLuint colorID = glGetUniformLocation(programID, "textColor");
		glUniform3f(colorID, color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		for (auto c = text.begin(); c != text.end(); c++) {

			Character ch = characters[*c];

			float scale = 1;
			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			float vertices[6][4] = {
				{ xpos, ypos + h, 0.0f, 0.0f },
				{ xpos, ypos, 0.0f, 1.0f },
				{ xpos + w, ypos, 1.0f, 1.0f },
				{ xpos, ypos + h, 0.0f, 0.0f },
				{ xpos + w, ypos, 1.0f, 1.0f },
				{ xpos + w, ypos + h, 1.0f, 0.0f }
			};

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			x += (ch.Advance >> 6) * scale;
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	

	static void Text2D::loadCharacters() {
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			cout << "Error initializing FreeType" << endl;
			return;
		}

		FT_Face face;
		if (FT_New_Face(ft, "../Fonts/ARIAL.TTF", 0, &face)) {
			cout << "Error loading font." << endl;
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, 48);

		for (unsigned char c = 0; c < 128; c++) {

			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				cout << "Error loading character glyph" << endl;
				continue;
			}

			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);

			// Texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character charecter = {
				texture,
				glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			characters.insert(pair<char, Character>(c, charecter));
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

	}


}