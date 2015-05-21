#include "BulletDebugDrawer.h"


#include "BulletDebugDrawer.h"

#include <stdio.h> //printf debugging
BulletDebugDrawer::BulletDebugDrawer()
{
	setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
}

BulletDebugDrawer::~BulletDebugDrawer()
{
}


// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::drawLine
// Purpose:       Draw a physics debug line
// Parameters:    Line origin, line end, origin color, end color
// --------------------------------------------------------------------------
void	BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	glBegin(GL_LINES);
	glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
	glVertex3d(from.getX(), from.getY(), from.getZ());
	glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
	glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::drawLine
// Purpose:       Draw a physics debug line
// Parameters:    Line origin, line end, color
// --------------------------------------------------------------------------
void	BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	drawLine(from, to, color, color);
}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::drawSphere
// Purpose:       Draw a physics debug sphere
// Parameters:    Position, radius, color
// --------------------------------------------------------------------------
void BulletDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
{
	glColor4f(color.getX(), color.getY(), color.getZ(), btScalar(1.0f));
	glPushMatrix();
	glTranslatef(p.getX(), p.getY(), p.getZ());

	int lats = 5;
	int longs = 5;

	int i, j;
	for (i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
		btScalar z0 = radius*sin(lat0);
		btScalar zr0 = radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}

	glPopMatrix();
}



// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::drawTriangle
// Purpose:       Draw a physics debug triangle
// Parameters:    First point, second point, third point, color, alpha
// --------------------------------------------------------------------------
void	BulletDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
{
	//	if (m_debugMode > 0)
	{
		const btVector3	n = btCross(b - a, c - a).normalized();
		glBegin(GL_TRIANGLES);
		glColor4f(color.getX(), color.getY(), color.getZ(), alpha);
		glNormal3d(n.getX(), n.getY(), n.getZ());
		glVertex3d(a.getX(), a.getY(), a.getZ());
		glVertex3d(b.getX(), b.getY(), b.getZ());
		glVertex3d(c.getX(), c.getY(), c.getZ());
		glEnd();
	}
}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::setDebugMode
// Purpose:       Set the debug mode
// Parameters:    The new debug mode (a binary OR of the enum btIDebugDraw::DebugDrawModes)
// --------------------------------------------------------------------------
void	BulletDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;

}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::getDebugMode
// Purpose:       Get the debug mode (a binary OR of the enum btIDebugDraw::DebugDrawModes)
// Parameters:    None
// --------------------------------------------------------------------------
int BulletDebugDrawer::getDebugMode() const
{
	return m_debugMode;
}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::draw3dText
// Purpose:       Draw debug text in the 3d world
// Parameters:    Location, text to render
// --------------------------------------------------------------------------
void	BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	glRasterPos3f(location.x(), location.y(), location.z());
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::reportErrorWarning
// Purpose:       Report an error
// Parameters:    Error text
// --------------------------------------------------------------------------
void	BulletDebugDrawer::reportErrorWarning(const char* warningString)
{
	WARNLOG("Debug Draw", warningString);
}

// --------------------------------------------------------------------------
// Function:      BulletDebugDrawer::drawContactPoint
// Purpose:       Draw a contact point between two objects
// Parameters:    Point, direction of line, line length, time to keep drawing line, color
// --------------------------------------------------------------------------
void	BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	btVector3 to = pointOnB + normalOnB * 1;//distance;
	const btVector3&from = pointOnB;
	glColor4f(color.getX(), color.getY(), color.getZ(), 1.f);
	glBegin(GL_LINES);
	{
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
	}
	glEnd();
}