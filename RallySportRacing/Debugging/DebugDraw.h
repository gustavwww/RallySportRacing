#include <btBulletDynamicsCommon.h>

class DebugDraw : public btIDebugDraw {
public:
		
		DebugDraw();

		void drawLine(const btVector3& from,
			const btVector3& to,
			const btVector3& color);

		int getDebugMode() const;

		void doDebugDraw();

		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

		void reportErrorWarning(const char* warningString);

		void draw3dText(const btVector3& location, const char* textString);

		void setDebugMode(int debugMode);

private:
		void setupDraw();
};
