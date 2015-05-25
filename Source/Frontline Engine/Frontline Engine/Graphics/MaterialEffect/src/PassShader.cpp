#include "PassGPUPipeline.h"
#include "PassShader.h"
#include "Pass.h"

Shader::Shader(const char* name)
	: PassDesc(FX_SHADER_DESC_FLAG,name)
{
}

Shader::~Shader()
{
}

bool Shader::LoadShaderFromFile(const char* fileName)
{
	std::string shaderCode;
	std::ifstream shaderStream(fileName, std::ios::in);
	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	m_Code = shaderCode;
	return true;
}

bool Shader::LoadShaderCode(const char* code)
{
	m_Code = code;
	return true;
}

void Shader::ValidateAttachments(std::shared_ptr<Pass> pass)
{
	std::shared_ptr<GPUPipeline> pipeline = std::static_pointer_cast<GPUPipeline>(pass->Find((fxState)FX_PIPELINE_DESC_FLAG));
	if (pipeline) {
		pipeline->Attach(shared_from_this());
		return;
	}
}