#include "DebugDraw.h"
#include <btBulletDynamicsCommon.h>
#include <vector>
#include "Rendering/SDLWindowHandler.h"
using namespace std;

// set up the line shader and then draw the buffer
GLuint VAO, VBO;

struct
{
	btVector3 p1;
	btVector3 p2;

} typedef LineValues;

vector<LineValues> TheLines;

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	// draws a simple line of pixels between points but stores them for later draw
	LineValues Line;
	Line.p1 = from;
	Line.p2 = to;

	TheLines.push_back(Line);

	// we don't care about colour?		
}

DebugDraw::DebugDraw()
{
	setupDraw();
}

void DebugDraw::setupDraw()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

void DebugDraw::doDebugDraw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, TheLines.size() * sizeof(LineValues), TheLines.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glDrawArrays(GL_LINES, 0, TheLines.size() * 2);
	TheLines.clear();
}

int DebugDraw::getDebugMode() const
{
	return DBG_DrawWireframe; // DBG_DrawWireframe|DBG_DrawAabb for both normal box and dtrawabb
}

void DebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void DebugDraw::reportErrorWarning(const char* warningString)
{
}

void DebugDraw::draw3dText(const btVector3& location, const char* textString)
{
}

void DebugDraw::setDebugMode(int debugMode)
{

}