#ifndef TEXT2D
#define TEXT2D

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <map>

using namespace std;

namespace Rendering {

    void loadCharacters();

    class Text2D {
    public:
	    Text2D(string text, glm::vec3 color, glm::vec2 pos);
	    void render(GLint programID);

    private:

        string text;
        glm::vec3 color;
        glm::vec2 pos;

        bool isTextSetup = false;
        void setupText();

        unsigned int VAO, VBO;
    };



    }



#endif