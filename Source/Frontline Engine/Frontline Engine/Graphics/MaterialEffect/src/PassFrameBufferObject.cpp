#include "PassFrameBufferObject.h"

FrameBufferObject::FrameBufferObject(const char* name)
	: PassDesc(FX_FBO_TARGET_DESC_FLAG, name)
{
}
