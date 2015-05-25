#pragma once
#include "PassState.h"
#include "flFxGL.h"

template<typename T>
struct State1 : public BlankState
{
	T m_v;
	State1(fxState state, T v)
		: BlankState(state)
	{
		m_v = v;
	}
	State1() {}
	virtual ~State1() {}
};
template<typename T1, typename T2>
struct State2 : public BlankState
{
	T1 m_v1;
	T2 m_v2;
	State2(fxState state, T1 v1, T2 v2)
		: BlankState(state)
	{
		m_v1 = v1;
		m_v2 = v2;
	}
	State2() {}
	virtual ~State2() {}
};

template<typename T1, typename T2, typename T3>
struct State3 : public BlankState
{
	T1 m_v1;
	T2 m_v2;
	T3 m_v3;
	State3(fxState state, T1 v1, T2 v2, T3 v3)
		: BlankState(state)
	{
		m_v1 = v1;
		m_v2 = v2;
		m_v3 = v3;
	}
	State3() {}
	virtual ~State3() {}

};

template<typename T1, typename T2, typename T3, typename T4>
struct State4 : public BlankState
{
	T1 m_v1;
	T2 m_v2;
	T3 m_v3;
	T4 m_v4;
	State4(fxState state, T1 v1, T2 v2, T3 v3, T4 v4)
		: BlankState(state)
	{
		m_v1 = v1;
		m_v2 = v2;
		m_v3 = v3;
		m_v4 = v4;
	}
	State4() {}
	virtual ~State4() {}
};

struct BlendGL : public State1<bool>
{
	BlendGL() {
		m_State = FX_BLEND;
	}
	bool Load(pugi::xml_node node) {
		m_v = node.attribute("blend").as_bool();
		return true;
	}
	bool Apply() {
		if (m_v == true)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		return true;
	}
	bool Cleanup() {
		if (m_v == true)
			glDisable(GL_BLEND);
		else
			glEnable(GL_BLEND);
		return true;
	}
};
struct BlendFuncGL : public State2<GLenum, GLenum>
{
	BlendFuncGL() {
		m_State = FX_BLEND_FUNC;
	}
	bool Load(pugi::xml_node node) {
		m_v1 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("src").value());
		m_v2 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("dst").value());
		return true;
	}
	bool Apply() {
		glBlendFunc(m_v1, m_v2);
		return true;
	}
};
struct AlphaFuncGL : public State2<GLenum, GLclampf>
{
	AlphaFuncGL() {
		m_State = FX_ALPHA_FUNC;
	}
	bool Load(pugi::xml_node node) {
		m_v1 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("func").value());
		m_v2 = StateInterpreter::Get()->Convert<GLclampf>(node.attribute("ref").value());
		return true;
	}
	bool Apply() {
		glAlphaFunc(m_v1, m_v2);
		return true;
	}
};
struct BlendFuncSeparateGL : public State4<GLenum, GLenum, GLenum, GLenum>
{
	BlendFuncSeparateGL() {
		m_State = FX_BLEND_FUNC_SEPARATE;
	}
	bool Load(pugi::xml_node node) {
		m_v1 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("srcRGB").value());
		m_v2 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("dstRGB").value());
		m_v3 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("srcAlpha").value());
		m_v4 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("dstAlpha").value());
		return true;
	}
	bool Apply() {
		glBlendFuncSeparate(m_v1, m_v2, m_v3, m_v4);
		return true;
	}
};
struct BlendEquationSeparateGL : public State2<GLenum, GLenum>
{
	BlendEquationSeparateGL() {
		m_State = FX_BLEND_EQUATION_SEPARATE;
	}
	bool Load(pugi::xml_node node) {
		m_v1 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("modeRGB").value());
		m_v2 = StateInterpreter::Get()->Convert<GLenum>(node.attribute("modeAlpha").value());
		return true;
	}
	bool Apply() {
		glBlendEquationSeparate(m_v1, m_v2);
		return true;
	}
};
struct LogicOpGL : public State1<GLenum>
{
	LogicOpGL() {
		m_State = FX_LOGIC_OP;
	}
	bool Load(pugi::xml_node node) {	
		m_v = StateInterpreter::Get()->Convert<GLenum>("opcode");
		return true;
	}
	bool Apply() {
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(m_v);
		return true;
	}
	bool Cleanup() {
		glDisable(GL_COLOR_LOGIC_OP);
		return true;
	}
};
struct ColorMaskGL : public State4<bool, bool, bool, bool>
{
	ColorMaskGL() {
		m_State = FX_COLOR_MASK;
	}
	bool Load(pugi::xml_node node) {
		m_v1 = node.attribute("red").as_bool();
		m_v2 = node.attribute("green").as_bool();
		m_v3 = node.attribute("blue").as_bool();
		m_v4 = node.attribute("alpha").as_bool();
		return true;
	}
	bool Apply() {
		GLboolean red = m_v1 ? GL_TRUE : GL_FALSE;
		GLboolean green = m_v2 ? GL_TRUE : GL_FALSE;
		GLboolean blue = m_v3 ? GL_TRUE : GL_FALSE;
		GLboolean alpha = m_v4 ? GL_TRUE : GL_FALSE;
		glColorMask(red, green, blue, alpha);
		return true;
	}
};
struct BlendColorGL : public State4<GLfloat, GLfloat, GLfloat, GLfloat>
{
	BlendColorGL() {
		m_State = FX_BLEND_COLOR;
	}
	bool Load(pugi::xml_node node) {
		m_v1 = StateInterpreter::Get()->Convert<GLfloat>(node.attribute("red").value());
		m_v2 = StateInterpreter::Get()->Convert<GLfloat>(node.attribute("green").value());
		m_v3 = StateInterpreter::Get()->Convert<GLfloat>(node.attribute("blue").value());
		m_v4 = StateInterpreter::Get()->Convert<GLfloat>(node.attribute("alpha").value());
		return true;
	}
	bool Apply() {
		glBlendColor(m_v1, m_v2, m_v3, m_v4);
		return true;
	}
};