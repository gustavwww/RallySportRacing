#ifndef TEXT2D
#define TEXT2D

#include <glm/glm.hpp>

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
	    Text2D();
	    void render();

        static void loadCharacters();

    private:
        static map<char, Character> characters;


    };



    }



#endif