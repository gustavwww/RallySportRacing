#include <GL/glew.h>

class FboInfo {
public:
	GLuint framebufferId;
	GLuint colorTextureTarget;
	GLuint depthBuffer;
	int width;
	int height;
	bool isComplete;

	FboInfo();
	FboInfo(int w, int h);

	void resize(int w, int h);
	bool checkFramebufferComplete(void);
};