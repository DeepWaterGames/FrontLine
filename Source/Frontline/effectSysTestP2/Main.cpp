#pragma once
#include "GL.h"
#include "flFxConfig.h"
#include "flFx.h"
#include "flFxExt.h"
#include "flFxGL.h"
#include "flFxParser.h"
#include "flFxParserGL.h"
#include "temp/LoadShaders.h"
#include "temp/texture.h"

struct TestData : public PassDesc {
	int var;
	TestData(fxState flag,const char* name) 
		: PassDesc(flag,name) {}
	virtual bool Validate() {std::cout<<"Validating..."<<std::endl;return true;}
	virtual bool Invalidate() {std::cout<<"Invalidating..."<<std::endl;return true;}
};
GLuint createFBO() {
	// Create and bind the FBO
	GLuint fboId, depth, pos, norm;

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	// The depth buffer
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 800);

	// The position buffer
	glActiveTexture(GL_TEXTURE0);   // Use texture unit 0
	glGenTextures(1, &pos);
	glBindTexture(GL_TEXTURE_2D, pos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The normal buffer
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &norm);
	glBindTexture(GL_TEXTURE_2D, norm);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Attach the images to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pos, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, norm, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fboId;
}
void blitFramebuffer(GLuint fboId, int sizeX, int sizeY) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, sizeX, sizeY, 0, sizeY / 2, sizeX, sizeY, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, sizeX, sizeY, 0, 0, sizeY, sizeY / 2, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
int main()
{
	//tests
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
	glewInit();
	flFxCoreGL::Get()->Init();
	pugi::xml_document doc;
	doc.load_file("effect.xml");
	std::shared_ptr<Effect> effect = flFxParserGL::Get()->LoadEffect(doc);
	std::cout<<effect->GetName()<<std::endl;
	std::cout<<effect->GetTechnique("Technique0")->GetName()<<std::endl;
	std::shared_ptr<Technique> demoTech = effect->GetTechnique("Technique0");
	std::shared_ptr<Pass> pass(new Pass("testPass"));
	std::shared_ptr<Pass> pass1(new Pass("testPass1"));
	std::shared_ptr<TestData> testData(new TestData(FX_DO_NOTHING,"testData"));
	std::shared_ptr<TestData> testData1(new TestData(FX_DO_NOTHING,"testData"));
	testData->var = 5;
	testData1->var = 19;
	pass->CreateData(testData);
	pass1->CreateDataOverride(testData1);
	pass->Validate();
	std::cout<<"Before: "<<(std::static_pointer_cast<TestData>(pass->Find("testData")))->var<<std::endl;
	pass1->SetupOverrides(&pass,1);
	pass->Validate();
	std::cout<<"After: "<<(std::static_pointer_cast<TestData>(pass->Find("testData")))->var<<std::endl;
	pass1->ReleaseOverrides(&pass,1);
	pass->Validate();
	std::cout<<"Now: "<<(std::static_pointer_cast<TestData>(pass->Find("testData")))->var<<std::endl;
	pass->RemoveData("testData");

	demoTech->Validate();

	std::shared_ptr<GPUPipeline> pipeline(new GPUPipeline());
	std::shared_ptr<Shader> shader(new Shader("testShader"));
	pass->CreateData(pipeline);
	pass->CreateData(shader);
	pass->Validate();

	std::shared_ptr<Shader> s1 = std::static_pointer_cast<Shader>(pass->Find("testShader"));
	if(s1 == pipeline->GetShader("testShader")) {
		std::cout<<"true"<<std::endl;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
	};
	static const GLfloat g_uv_buffer_data[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		0.0f, 1.0f,
	};
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	/*std::shared_ptr<Pass> demoPass(new Pass("demoPass"));

	std::shared_ptr<GLSLShader> glslVertShader(new GLSLShader("shader1",GL_VERTEX_SHADER));
	std::shared_ptr<GLSLShader> glslFragShader(new GLSLShader("shader2",GL_FRAGMENT_SHADER));
	glslVertShader->LoadShaderFromFile("VertShader.glsl");
	//glslVertShader->Validate();
	glslFragShader->LoadShaderFromFile("FragShader.glsl");
	//glslFragShader->Validate();
	std::shared_ptr<PipelineGL> glslPipeline(new PipelineGL());
	//glslPipeline->SetShader(glslVertShader);
	//glslPipeline->SetShader(glslFragShader);
	//glslPipeline->Validate();
	demoPass->CreateData(glslPipeline);
	demoPass->CreateData(glslVertShader);
	demoPass->CreateData(glslFragShader); //order of validation screwed up
	demoPass->Validate();
	std::cout<<typeid(*demoPass.get()).name()<<std::endl; //maybe make ALL the flags typeid*/
	//maybe make it so when using Pass::Find on flags, just put the class type u want to recieve

	glm::vec3 color(1.0f,0.0f,0.0f);
	std::shared_ptr<ShaderParameterGL> uniform(new ShaderParameterGL("uniform_color",ShaderParameterGL::Type::T3F,glm::value_ptr(color))); //count maybe cud be dyn. generated
	/*std::shared_ptr<StateGroupGL> states(new StateGroupGL());
	std::shared_ptr<State1<bool>> blend(new State1<bool>(FX_BLEND,true));
	std::shared_ptr<State2<GLenum,GLenum>> blendFunc(new State2<GLenum,GLenum>(FX_BLEND_FUNC,GL_ONE,GL_ONE));
	states->AddState(blend);
	states->AddState(blendFunc);*/
	demoTech->GetPass(0)->CreateData(uniform);
	//demoTech->GetPass(0)->CreateData(states);

	State1<bool> fx_blend(FX_BLEND,true);
	State2<GLenum,GLenum>fx_blendfunc(FX_BLEND_FUNC,GL_ONE,GL_ONE);

	std::cout << "ATTENTION: STARTING REPLACEMENT FUNCTION TESTING!" << std::endl;
	std::shared_ptr<Pass> demoPass2(new Pass("demoPass2"));
	demoPass2->Validate();
	std::shared_ptr<PassDesc> demoPassDesc1(new PassDesc(FX_DO_NOTHING, "demo1"));
	demoPass2->CreateData(demoPassDesc1);
	std::shared_ptr<PassDesc> demoPassRandom(new PassDesc(FX_CLEAR_DESC_FLAG, "random"));
	demoPass2->CreateData(demoPassRandom);
	std::cout << "Before: " << demoPass2->Find(FX_DO_NOTHING)->GetName() << std::endl;
	std::shared_ptr<PassDesc> demoPassDesc2(new PassDesc(FX_DO_NOTHING, "demo2"));
	demoPass2->Replace("demo1", demoPassDesc2);
	std::cout << "After: " << demoPass2->Find(FX_DO_NOTHING)->GetName() << std::endl;
	
	//GLuint uvBuffer;
	//glGenBuffers(1, &uvBuffer);
	GLuint textureId = loadTexture("texture.bmp");
	//std::shared_ptr<ResourceGL> resource(new ResourceGL("res1", FX_RESOURCE_2D));
	//resource->SetTexture(textureId, 0);
	//demoTech->GetPass(0)->CreateData(resource);

	std::cout << "***********RESOURCE TESTING************" << std::endl;
	std::shared_ptr<ResourceGL> resource = std::static_pointer_cast<ResourceGL>(demoTech->GetPass(0)->Find("resource"));
	if (resource) {
		std::cout << "resource exists: " << resource->GetName() << std::endl;
		std::cout << "is valid before: " << resource->IsValid() << std::endl;
		resource->SetTexture(textureId);
		std::cout << "is valid after: " << resource->IsValid() << std::endl;
	}
	/*std::shared_ptr<SamplerStateGL> samplerState(new SamplerStateGL("sstate0"));
	samplerState->Validate();
	std::shared_ptr<State2<GLenum, GLenum>> minFilter(new State2<GLenum,GLenum>(FX_TEXTURE_MIN_FILTER, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	std::shared_ptr<State2<GLenum, GLenum>> magFilter(new State2<GLenum, GLenum>(FX_TEXTURE_MAG_FILTER, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	std::shared_ptr<State2<GLenum, GLenum>> texWrapS(new State2<GLenum, GLenum>(FX_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_S, GL_REPEAT));
	std::shared_ptr<State2<GLenum, GLenum>> texWrapT(new State2<GLenum, GLenum>(FX_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_T, GL_REPEAT));
	samplerState->AddState(minFilter);
	samplerState->AddState(magFilter);
	samplerState->AddState(texWrapS);
	samplerState->AddState(texWrapT);
	resource->SetSamplerState(samplerState);*/

	//StateInterpreter::Get()->RegisterState(FX_LINEAR, GL_LINEAR, "FX_LINEAR");
	//StateInterpreter::Get()->RegisterState(FX_RESOURCE_2D, GL_TEXTURE_2D, "FX_RESOURCE_2D");
	GLenum texture2D = StateInterpreter::Get()->Convert<GLenum>(FX_RESOURCE_2D);
	fxState fxTexture2D = StateInterpreter::Get()->Convert(GL_TEXTURE_2D);
	if ((texture2D == GL_TEXTURE_2D) && (fxTexture2D == FX_RESOURCE_2D)) {
		std::cout << "it is a GL_TEXTURE_2D" << std::endl;
		std::cout << "its a: " << StateInterpreter::Get()->ToString(FX_RESOURCE_2D) << std::endl;
	}
	std::cout << "GLenum: " << texture2D << "," << GL_TEXTURE_2D << std::endl;
	std::cout << "fxState: " << fxTexture2D << "," << FX_RESOURCE_2D << std::endl;

	State2<fxState, fxState> state(FX_BLEND_FUNC, FX_ONE, FX_ONE);
	State3<float, float, float> blendColor(FX_BLEND_COLOR, 1.0f, 0.0f, 0.0f);
	//glBlendColor(blendColor.m_v1, blendColor.m_v2, blendColor.m_v3, 1.0f);
	//glBlendFunc(spm.Convert<GLenum>(state.m_v1), spm.Convert<GLenum>(state.m_v2));

	/*std::shared_ptr<FrameBufferObjectGL> framebuffer(new FrameBufferObjectGL("fbo0"));
	demoTech->GetPass(0)->CreateData(framebuffer);

	std::shared_ptr<ResourceGL> posTex(new ResourceGL("posTex", FX_RESOURCE_RENDERTEX));
	posTex->SetFormat(FX_RGB32F);
	posTex->SetDimensions(800, 800);
	posTex->SetFBOAttachment(0);
	posTex->Validate();
	//demoTech->GetPass(0)->CreateData(posTex);

	std::shared_ptr<ResourceGL> diffTex(new ResourceGL("diffTex", FX_RESOURCE_RENDERTEX));
	diffTex->SetFormat(FX_RGB32F);
	diffTex->SetDimensions(800, 800);
	diffTex->SetFBOAttachment(1);
	diffTex->Validate();
	//demoTech->GetPass(0)->CreateData(diffTex);

	std::shared_ptr<ResourceGL> depthTex(new ResourceGL("depthTex", FX_RESOURCE_DEPTHTEX));
	depthTex->SetFormat(FX_DEPTH_COMPONENT);
	depthTex->SetDimensions(800, 800);
	depthTex->Validate();
	//demoTech->GetPass(0)->CreateData(depthTex);

	framebuffer->AddTextureResource(posTex);
	framebuffer->AddTextureResource(diffTex);
	framebuffer->SetDSTTexture(depthTex);*/
	

	//GLuint fbo = createFBO();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Pass::Begin();
		//demoPass->Execute();
		//framebuffer->SetCurrent();
		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		demoTech->GetPass(0)->Execute();
		//glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBindVertexArray(VertexArrayID); 
		/*glVertexAttribPointer(
		   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);*/
		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		//glDisableVertexAttribArray(0);
		//std::shared_ptr<PipelineGL> pipeline = FX_CAST(PipelineGL)(demoTech->GetPass(0)->Find(FX_PIPELINE_DESC_FLAG));
		//flFxCoreGL::Get()->DrawFullscreenQuad();
		//blitFramebuffer(std::static_pointer_cast<FrameBufferObjectGL>(demoTech->GetPass(0)->Find(FX_FBO_TARGET_DESC_FLAG))->GetFBOId()/*framebuffer->GetFBOId() */, 800, 800);
		//blitFramebuffer(framebuffer->GetFBOId(), 800, 800);
		demoTech->GetPass(0)->Cleanup();
		demoTech->GetPass(1)->Execute();
		demoTech->GetPass(1)->Cleanup();
		/*bool enabled = glIsEnabled(GL_BLEND);
		std::cout << "Blend Status: " << enabled << std::endl;*/
		//pipeline->Unbind();
		window.display();


    }
	
    return 0;
}