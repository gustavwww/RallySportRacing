#ifndef TEXT2D
#define TEXT2D

#include <glm/glm.hpp>
#include <string>
#include <map>

using namespace std;

namespace Rendering {

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::vec2   Size;       // Size of glyph
        glm::vec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    class Text2D {
    public:
	    Text2D(string text, glm::vec3 color);
	    void render(GLint programID);

        static void loadCharacters();

    private:
        string text;
        glm::vec3 color;

        static map<char, Character> characters;
        bool isTextSetup = false;
        void setupText();

        unsigned int VAO, VBO;
    };



    }



#endif