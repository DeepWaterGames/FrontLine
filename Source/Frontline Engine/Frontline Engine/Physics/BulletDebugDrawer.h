#pragma once
#include "..\FrontlineCommon.h"
#include <PHYSICS\LinearMath\btIDebugDraw.h>
#include <gl/GL.h>
class BulletDebugDrawer :
	public btIDebugDraw
{
	int m_debugMode;
public:
	BulletDebugDrawer();
	~BulletDebugDrawer();
	virtual void drawLine(const btVector3& pv_from, const btVector3& pv_to, const btVector3& pv_color) override;
	virtual void drawLine(const btVector3& pv_from, const btVector3& pv_to, const btVector3& pv_colorFrom, const btVector3& pv_colorTo) override;
	virtual void drawContactPoint(const btVector3& pv_point, const btVector3& pv_normal, btScalar pf_distance, int pi_lifeTime, const btVector3& pv_color) override;
	virtual void reportErrorWarning(const char* ps_warningString) override;
	virtual void setDebugMode(int pi_mode) override;
	virtual int getDebugMode() const override;
	virtual void draw3dText(const btVector3& pv_location, const char* ps_textString) override;
	virtual void drawTriangle(const btVector3& pv_vert1, const btVector3& pv_vert2, const btVector3& pv_vert3, const btVector3& pv_color, btScalar pv_alpha);
	virtual void drawSphere(const btVector3& pv_center, btScalar pf_radius, const btVector3& pv_color);
};