#include "PassDesc.h"
#include "Pass.h"
#include "PassResource.h"
#include "PassFrameBufferObject.h"

bool SamplerState::AddState(std::shared_ptr<BlankState> state) 
{
	for (auto iter = m_States.begin(); iter != m_States.end(); iter++) 
	{
		if ((*iter) == state) 
		{
			return false;
		}
	}
	m_States.push_back(state);
	return true;
}

std::shared_ptr<BlankState> SamplerState::FindState(fxState state) 
{
	for (auto iter = m_States.begin(); iter != m_States.end(); iter++) 
	{
		if ((*iter)->m_State = state) 
		{
			return (*iter);
		}
	}
	return NULL;
}

bool SamplerState::RemoveState(fxState state) 
{
	for (auto iter = m_States.begin(); iter != m_States.end();) 
	{
		if ((*iter)->m_State == state) 
		{
			m_States.erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}

Resource::Resource(const char* name, fxState resType)
: PassDesc(FX_RESOURCE_DESC_FLAG, name) 
{
	m_Data.appDepSize = false;
	m_Data.msaa = false;
	m_Data.external = false;
	m_ResourceType = resType;
	attached = false;
	//flFxCore::Get().GetResourceRepository()->AddItem(this);
}

Resource::~Resource() 
{
	//flFxCore::Get().GetResourceRepository()->RemoveItem(this);
}

void Resource::SetDimensions(int w, int h, int d) 
{
	m_Data.dimensions[0] = w;
	m_Data.dimensions[1] = h;
	m_Data.dimensions[2] = d;
}

void Resource::ValidateAttachments(std::shared_ptr<Pass> pass) 
{
	if(attached)
		return;

	switch (m_ResourceType) {
	case FX_RESOURCE_RENDERTEX:
	{
		std::shared_ptr<FrameBufferObject> framebuffer = std::static_pointer_cast<FrameBufferObject>(pass->Find((fxState)FX_FBO_TARGET_DESC_FLAG));
		if (framebuffer) 
		{
			framebuffer->Attach(shared_from_this());
		}
		break;
	}
	case FX_RESOURCE_DEPTHTEX:
	{
		std::shared_ptr<FrameBufferObject> framebuffer = std::static_pointer_cast<FrameBufferObject>(pass->Find((fxState)FX_FBO_TARGET_DESC_FLAG));
		if (framebuffer) 
		{
			framebuffer->Attach(shared_from_this());
		}	
		break;
	}
	}
	attached = true;
}

void Resource::InvalidateAttachments(std::shared_ptr<Pass> pass)
{
	attached = false;
	PassDesc::InvalidateAttachments(pass);
}