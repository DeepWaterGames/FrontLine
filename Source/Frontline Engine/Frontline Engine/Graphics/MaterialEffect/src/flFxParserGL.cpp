#include "flFxParserGL.h"


flFxParserGL::flFxParserGL()
{
	RegisterXmlNodeHandle("Effect", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleEffectNode));
	RegisterXmlNodeHandle("Technique", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleTechniqueNode));
	RegisterXmlNodeHandle("Pass", fastdelegate::MakeDelegate(this, &flFxParserGL::HandlePassNode));
	RegisterXmlNodeHandle("VertexShaderBody", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleShaderBodyNode));
	RegisterXmlNodeHandle("FragmentShaderBody", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleShaderBodyNode));
	RegisterXmlNodeHandle("FrameBufferObjectBody", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleFrameBufferObjectBodyNode));
	RegisterXmlNodeHandle("ResourceBody", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleResourceBodyNode));
	RegisterXmlNodeHandle("State", fastdelegate::MakeDelegate(this, &flFxParserGL::HandleStateNode));
}

flFxParserGL::~flFxParserGL()
{
}
static flFxParserGL* parserGL = new flFxParserGL();
flFxParserGL* flFxParserGL::Get() {
	return parserGL;
}
bool flFxParserGL::HandleEffectNode(pugi::xml_node node) {
	m_TmpEffect = std::shared_ptr<Effect>(new Effect(node.attribute("name").value()));
	return true;
}
bool flFxParserGL::HandleTechniqueNode(pugi::xml_node node) {
	std::shared_ptr<Technique> technique(new Technique(node.attribute("name").value()));
	m_TmpEffect->AddTechnique(technique);
	m_CurrTechnique = technique;
	return true;
}
bool flFxParserGL::HandlePassNode(pugi::xml_node node) {
	std::shared_ptr<Pass>pass(new Pass(node.attribute("name").value()));
	std::shared_ptr<PipelineGL>pipeline(new PipelineGL());
	std::shared_ptr<StateGroupGL>stateGroup(new StateGroupGL());
	std::shared_ptr<FrameBufferObjectGL> fboCopy = NULL;
	fxState clear[3];
	pass->CreateData(pipeline);
	pass->CreateData(stateGroup);
	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		if ((strcmp(child.name(), "VertexShader") == 0) || (strcmp(child.name(), "FragmentShader") == 0)) {
			std::shared_ptr<GLSLShader> shader = std::static_pointer_cast<GLSLShader>(GetQueuedObject(child.first_child().value()));
			if (shader) {
				std::shared_ptr<GLSLShader> shaderCopy(new GLSLShader(*shader));
				//shaderCopy->SetName(child.attribute("name").value()); //remove this line
				pass->CreateData(shaderCopy);
			}
			continue;
		}
		if (strcmp(child.name(), "Resource") == 0) {
			//check the saved object list first
			std::shared_ptr<ResourceGL> resource = std::static_pointer_cast<ResourceGL>(GetSavedObject(child.first_child().value()));
			if (resource) {
				pass->CreateData(resource); //this is the render texture filled by the fbo of the previous pass
			}
			else {
				std::shared_ptr<ResourceGL> resource = std::static_pointer_cast<ResourceGL>(GetQueuedObject(child.first_child().value()));
				if (resource) {
					std::shared_ptr<ResourceGL> resourceCopy(new ResourceGL(*resource));
					//resourceCopy->SetName(child.attribute("name").value()); //remove this item
					pass->CreateData(resourceCopy);
				}
			}
			continue;
		}
		if (strcmp(child.name(), "RenderTarget") == 0) {
			const char* targetName = child.first_child().value();
			if (strcmp(targetName, "FX_BACKBUFFER") == 0) {
				continue;
			}
			else { //if this is framebuffer
				std::shared_ptr<FrameBufferObjectGL> fbo = std::static_pointer_cast<FrameBufferObjectGL>(GetQueuedObject(targetName));
				if (fbo) {
					fboCopy = std::shared_ptr<FrameBufferObjectGL>(new FrameBufferObjectGL(fbo->GetName()));

					pass->CreateData(fboCopy); //before attaching, the fbo has to be part of a pass

					std::vector<std::shared_ptr<ResourceGL>> savedResources;
					for (int i = 0; i < GetNumSavedObjects(); i++)
					{
						std::shared_ptr<ResourceGL> res = std::dynamic_pointer_cast<ResourceGL>(GetSavedObject(i));
						//if (res && ((res->GetResourceType() == FX_RESOURCE_RENDERTEX) || (res->GetResourceType() == FX_RESOURCE_DEPTHTEX)))
						if (res)
						{
							savedResources.push_back(res);
						}
					}

					for (auto iter = savedResources.begin(); iter != savedResources.end(); iter++) {
						std::shared_ptr<ResourceGL> tex = *iter;// (new ResourceGL(*(*iter)));
						/*if (tex->GetResourceType() == FX_RESOURCE_RENDERTEX) 
						{*/
							//SaveObject(renderTex); //instead of SaveObject, use the repository
							//fboCopy->AddTextureResource(renderTex);
							//fboCopy->Attach(renderTex);
							//i mean... temp for now, use the old method for fbo...
							pass->StoreData(tex); //DO NOT ADD TO PASS CUZ IT DOESNT NEED BINDING
							//renderTex->SetActive(false); //you dont want it to execute here...
						/*}
						else if (tex->GetResourceType() == FX_RESOURCE_DEPTHTEX)
						{*/
							//SaveObject(depthTex);
							//fboCopy->SetDSTTexture(depthTex);
							//fboCopy->Attach(depthTex);
							//pass->StoreData(tex);
							//depthTex->SetActive(false);
						//}
					}
					//pass->CreateData(fboCopy);
				}
			}
			continue;
		}
		if (strcmp(child.name(), "ClearMode") == 0) {
			const char* clear1 = child.attribute("clear1").value();
			const char* clear2 = child.attribute("clear2").value();
			const char* clear3 = child.attribute("clear3").value();
			if (strcmp(clear1, "") != 0) {
				clear[0] = StateInterpreter::Get()->ToState(clear1);
			}
			else {
				clear[0] = NULL;
			}
			if (strcmp(clear2, "") != 0) {
				clear[1] = StateInterpreter::Get()->ToState(clear2);
			}
			else {
				clear[1] = NULL;
			}
			if (strcmp(clear3, "") != 0) {
				clear[2] = StateInterpreter::Get()->ToState(clear3);
			}
			else {
				clear[2] = NULL;
			}
			continue;
		}
		if (strcmp(child.name(), "RenderingMode") == 0) {
			const char* mode = child.attribute("mode").value();
			std::shared_ptr<RenderingModeGL> renderingMode(new RenderingModeGL(StateInterpreter::Get()->ToState(mode)));
			pass->CreateData(renderingMode);
			continue;
		}
	}
	if (fboCopy) {
		fboCopy->SetClearMode(clear[0], clear[1], clear[2]);
	}
	m_CurrTechnique->AddPass(pass);
	m_CurrPass = pass;
	return true;
}
bool flFxParserGL::HandleShaderBodyNode(pugi::xml_node node) {
	if (strcmp(node.name(), "VertexShaderBody") == 0) {
		std::shared_ptr<GLSLShader> vertexShader(new GLSLShader(node.attribute("name").value(), GL_VERTEX_SHADER, node.first_child().value()));
		SetQueuedObject(vertexShader);
	}
	else if (strcmp(node.name(), "FragmentShaderBody") == 0) {
		std::shared_ptr<GLSLShader> fragmentShader(new GLSLShader(node.attribute("name").value(), GL_FRAGMENT_SHADER, node.first_child().value()));
		SetQueuedObject(fragmentShader);
	}
	return true;
}
bool flFxParserGL::HandleResourceBodyNode(pugi::xml_node node) {
	fxState type = NULL;
	//try to find a way to loop through the different types of resources so u dont have to manually specify
	if (strcmp(node.attribute("type").value(), "FX_RESOURCE_2D") == 0) {
		type = FX_RESOURCE_2D;
	}
	if (strcmp(node.attribute("type").value(), "FX_RESOURCE_RENDERTEX") == 0) {
		type = FX_RESOURCE_RENDERTEX;
	}
	if (strcmp(node.attribute("type").value(), "FX_RESOURCE_DEPTHTEX") == 0) {
		type = FX_RESOURCE_DEPTHTEX;
	}
	if (!type) {
		return false;
	}
	std::shared_ptr<ResourceGL> resource(new ResourceGL(node.attribute("name").value(), type));
	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		if (strcmp(child.name(), "Texture") == 0) {
			int unit = child.attribute("binding").as_int();
			resource->SetTexUnit(unit);
			continue;
		}
		if (strcmp(child.name(), "SamplerState") == 0) {
			std::shared_ptr<SamplerStateGL> samplerState(new SamplerStateGL(child.attribute("name").value()));
			for (pugi::xml_node sstateChild = child.first_child(); sstateChild; sstateChild = sstateChild.next_sibling()) {
				if (strcmp(sstateChild.name(), "FX_TEXTURE_MIN_FILTER") == 0) {
					std::shared_ptr<State2<GLenum, GLenum>> minFilter(new State2<GLenum, GLenum>(FX_TEXTURE_MIN_FILTER, GL_TEXTURE_MIN_FILTER, StateInterpreter::Get()->Convert<GLenum>(StateInterpreter::Get()->ToState(sstateChild.attribute("value").value()))));
					samplerState->AddState(minFilter);
					continue;
				}
				if (strcmp(sstateChild.name(), "FX_TEXTURE_MAG_FILTER") == 0) {
					std::shared_ptr<State2<GLenum, GLenum>> magFilter(new State2<GLenum, GLenum>(FX_TEXTURE_MAG_FILTER, GL_TEXTURE_MAG_FILTER, StateInterpreter::Get()->Convert<GLenum>(StateInterpreter::Get()->ToState(sstateChild.attribute("value").value()))));
					samplerState->AddState(magFilter);
					continue;
				}
				if (strcmp(sstateChild.name(), "FX_TEXTURE_WRAP_S") == 0) {
					std::shared_ptr<State2<GLenum, GLenum>> texWrapS(new State2<GLenum, GLenum>(FX_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_S, StateInterpreter::Get()->Convert<GLenum>(StateInterpreter::Get()->ToState(sstateChild.attribute("value").value()))));
					samplerState->AddState(texWrapS);
					continue;
				}
				if (strcmp(sstateChild.name(), "FX_TEXTURE_WRAP_T") == 0) {
					std::shared_ptr<State2<GLenum, GLenum>> texWrapT(new State2<GLenum, GLenum>(FX_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_T, StateInterpreter::Get()->Convert<GLenum>(StateInterpreter::Get()->ToState(sstateChild.attribute("value").value()))));
					samplerState->AddState(texWrapT);
					continue;
				}
			}
			samplerState->Validate();
			resource->SetSamplerState(samplerState);
			continue;
		}
		//now for the render textures
		if (strcmp(child.name(), "InternalFormat") == 0) {
			fxState internalFormat = StateInterpreter::Get()->ToState(child.attribute("format").value());
			std::cout << child.attribute("format").value() << ":" << internalFormat << std::endl;
			resource->SetFormat(internalFormat);
			continue;
		}
		if (strcmp(child.name(), "Size") == 0) {
			int w = child.attribute("w").as_int();
			int h = child.attribute("h").as_int();
			int d = child.attribute("d").as_int();
			resource->SetDimensions(w, h, d);
			continue;
		}
	}
	SetQueuedObject(resource);
	return true;
}
bool flFxParserGL::HandleFrameBufferObjectBodyNode(pugi::xml_node node) {
	std::shared_ptr<FrameBufferObjectGL> fbo(new FrameBufferObjectGL(node.attribute("name").value()));
	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		if (strcmp(child.name(), "RenderTexture") == 0) {
			int attachment = child.attribute("attachment").as_int();
			const char* resName = child.first_child().value();
			std::shared_ptr<ResourceGL> res = std::static_pointer_cast<ResourceGL>(GetQueuedObject(resName));
			if (res) {
				std::shared_ptr<ResourceGL> resCopy(new ResourceGL(*res));
				resCopy->SetFBOAttachment(attachment);
				SaveObject(resCopy); //to be accessed for later use
				//fbo->Attach(resCopy);
			}
			continue;
		}
		if (strcmp(child.name(), "DepthTexture") == 0) {
			const char* resName = child.first_child().value();
			std::shared_ptr<ResourceGL> res = std::static_pointer_cast<ResourceGL>(GetQueuedObject(resName));
			if (res) {
				std::shared_ptr<ResourceGL> resCopy(new ResourceGL(*res));
				SaveObject(resCopy); //to be accessed for later use
				//fbo->Attach(resCopy);
			}
			continue;
		}
	}
	SetQueuedObject(fbo);
	return true;
}
bool flFxParserGL::HandleStateNode(pugi::xml_node node) {
	const char* name = node.attribute("type").value();
	std::shared_ptr<StateGroupGL> stateGroup = std::static_pointer_cast<StateGroupGL>(m_CurrPass->Find((fxState)FX_STATE_GROUP_DESC_FLAG));
	std::shared_ptr<BlankState> state;
	if (strcmp(name, "FX_BLEND") == 0) {
		state = std::shared_ptr<BlendGL>(new BlendGL());
	}
	if (strcmp(name, "FX_BLEND_FUNC") == 0) {
		state = std::shared_ptr<BlendFuncGL>(new BlendFuncGL());
	}
	if (strcmp(name, "FX_ALPHA_FUNC") == 0) {
		state = std::shared_ptr<AlphaFuncGL>(new AlphaFuncGL());
	}
	if (strcmp(name, "FX_BLEND_FUNC_SEPARATE") == 0) {
		state = std::shared_ptr<BlendFuncSeparateGL>(new BlendFuncSeparateGL());
	}
	if (strcmp(name, "FX_BLEND_EQUATION_SEPARATE") == 0) {
		state = std::shared_ptr<BlendEquationSeparateGL>(new BlendEquationSeparateGL());
	}
	if (strcmp(name, "FX_LOGIC_OP") == 0) {
		state = std::shared_ptr<LogicOpGL>(new LogicOpGL());
	}
	if (strcmp(name, "FX_COLOR_MASK") == 0) {
		state = std::shared_ptr<ColorMaskGL>(new ColorMaskGL());
	}
	if (strcmp(name, "FX_BLEND_COLOR") == 0) {
		state = std::shared_ptr<BlendColorGL>(new BlendColorGL());
	}
	if (state) {
		state->Load(node);
		stateGroup->AddState(state);
	}
	return true;
}