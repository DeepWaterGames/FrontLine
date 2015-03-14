#pragma once
#include <map>
#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <stack>
#include "GL.h"
#include "flFxConfig.h"

class PassDesc;
class Pass;
class Technique;
class Effect;
class Repository;

typedef unsigned int fxState;

class flFxCore
{
public:
	virtual bool Init() { return true; }
	virtual bool Shutdown() { return true; }
	virtual void DrawFullscreenQuad() {}
	virtual void DrawTriangle(int x, int y, int z) {}
};
class PassDesc 
	: public std::enable_shared_from_this<PassDesc>
{
protected:
	struct Dependency {
		Dependency()
		: flag(NULL), name(NULL) {}
		const char* name;
		fxState flag;
	};
	bool m_Active;
	bool m_Validated;
	bool m_Modified; //if the PassDesc has been modified since validation
	const char* m_Name;
	fxState m_Flag;
	std::weak_ptr<Pass> m_Parent;
	std::vector<Dependency> m_Owners;
	std::vector<Dependency> m_Data;
public:
	PassDesc(fxState flag,const char* name = NULL);
	virtual ~PassDesc();
	virtual void SetName(const char* name) {m_Name = name;}
	virtual const char* GetName() {return m_Name;}
	virtual void SetFlag(fxState flag) {m_Flag = flag;}
	virtual fxState GetFlag() {return m_Flag;}
	virtual void SetActive(bool active) { m_Active = active; }
	virtual bool IsActive() { return m_Active; }
	virtual bool IsValid() {return m_Validated;}
	virtual bool HasChanged() { return m_Modified; }
	virtual void SetParent(std::shared_ptr<Pass> parent) {m_Parent = parent;}
protected:
	virtual void SetValidated(bool validated) { m_Validated = validated; }

	virtual bool Attach(std::shared_ptr<PassDesc> dep) { return true; } //atach the shit on create data
	virtual bool Attach(const char* name) { return true; }
	virtual bool Attach(fxState flag) { return true; }
	virtual bool Detach(std::shared_ptr<PassDesc> dep) { return true; }
	virtual bool Detach(const char* name) { return true; }
	virtual bool Detach(fxState flag) { return true; }
	virtual bool AttachTo(std::shared_ptr<PassDesc> owner) { return true; } //creates a dependency ref to it
	virtual bool AttachTo(const char* name) { return true; }
	virtual bool AttachTo(fxState flag) { return true; }
	virtual bool DetachFrom(std::shared_ptr<PassDesc> owner) { return true; }
	virtual bool DetachFrom(const char* name) { return true; }
	virtual bool DetachFrom(fxState flag) { return true; }

	virtual std::shared_ptr<PassDesc> GetFromDependency(Dependency dep) { return nullptr; }
	virtual void GetFromDependency(Dependency dep, std::vector<Dependency> deps) {}

	virtual bool OnCreateData() { return true; } //do attachments, not called in CreateData of pass
	virtual void RunAttachments() {} //called internally in Pass::Validate to attach everything
	virtual bool Validate() { SetValidated(true); return true; }
	//virtual bool ValidateImpl() { return true; }
	virtual bool Execute() { return true; } 
	virtual bool Cleanup() { return true; } 
	virtual bool Invalidate() { SetValidated(false); return true; }
	//virtual bool InvalidateImpl() { return true; }
	virtual bool OnDestroyData() { return true; } //detach the shit
friend class Pass;
};
class Pass
	: public std::enable_shared_from_this<Pass>
{
protected:
	typedef std::vector<std::shared_ptr<PassDesc>> PassDataVec; 
	struct Layer {
		Layer() {}
		~Layer() {} 
		PassDataVec dataForValidation;
		PassDataVec dataForExecution;
	};
	std::map<int,Layer> m_Layers;
	PassDataVec m_Overrides;
	int m_BaseLayer;
	int m_ActiveLayer;
	std::weak_ptr<Technique> m_Parent;
	std::map<fxState,std::vector<std::shared_ptr<PassDesc>>> m_PassInfo;
	static std::vector<fxState> m_ExecutionConfig;
	std::map<std::shared_ptr<Pass>,int> m_OverrideIds;
	bool m_Validated;
	const char* m_Name;
public:
	Pass(const char* name);
	~Pass();
	void SetParent(std::shared_ptr<Technique> parent) {m_Parent = parent;}
	void SetName(const char* name) {m_Name = name;}
	const char* GetName() {return m_Name;}
	bool IsValid() {return m_Validated;}

	static void ConfigureExecution(std::vector<fxState> flags) { m_ExecutionConfig = flags; }

	bool Validate(); 
	bool Execute(); 
	bool Cleanup();
	bool Invalidate();

	void SetActiveLayer(int layer, bool copyFromPrev); 

	void CreateData(std::shared_ptr<PassDesc> data); 
	void CreateDataOverride(std::shared_ptr<PassDesc> data); 

	void Replace(std::shared_ptr<PassDesc> data);
	void Replace(const char* name, std::shared_ptr<PassDesc> data); 
	
	std::shared_ptr<PassDesc> Find(const char* name); 
	std::shared_ptr<PassDesc> Find(fxState flag);
	void Find(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data);

	std::shared_ptr<PassDesc> FindOverride(const char* name);
	std::shared_ptr<PassDesc> FindOverride(fxState flag);
	void FindOverride(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data);

	bool RemoveData(std::shared_ptr<PassDesc> data);
	bool RemoveData(const char* name);
	bool RemoveData(fxState flag);
	void ClearData() {m_Layers[m_ActiveLayer].dataForExecution.clear();}

	void SetupOverrides(std::shared_ptr<Technique>* dest,int numTechs);
	void SetupOverrides(std::shared_ptr<Pass>* dest,int numPasses);
	void ReleaseOverrides(std::shared_ptr<Technique>* dest,int numTechs);
	void ReleaseOverrides(std::shared_ptr<Pass>* dest,int numPasses);
	bool RemoveLayer(int layer);
	bool RemoveActiveLayer();

	Layer& GetLayer() {return m_Layers[m_ActiveLayer];}
	Layer& GetBaseLayer() {return m_Layers[0];}
	int GetActiveLayerId() {return m_ActiveLayer;}
friend class PassDesc;
};
class Technique
	: public std::enable_shared_from_this<Technique>
{
private:
	struct PassInfo 
	{
		std::shared_ptr<Pass> pass;
		bool isActive;
	};
	std::vector<PassInfo> m_Passes;
	const char* m_Name;
public:
	Technique(const char* name) {m_Name = name;}
	~Technique();
	void SetName(const char* name) {m_Name = name;}
	const char* GetName() {return m_Name;}
	bool Validate();
	bool Invalidate();
	bool AddPass(std::shared_ptr<Pass> pass);
	bool RemovePass(std::shared_ptr<Pass> pass);
	int GetNumPasses() {return (int)m_Passes.size();}
	void SetPassActive(int index,bool active);
	void SetPassActive(const char* name,bool active);
	bool GetPassActive(int index);
	bool GetPassActive(const char* name);
	std::shared_ptr<Pass> GetPass(int index);
	std::shared_ptr<Pass> GetPass(const char* name);
};
class Effect
	: public std::enable_shared_from_this<Effect>
{
private:
	std::vector<std::shared_ptr<Technique>>m_Techniques;
	const char* m_Name;
public:
	Effect(const char* name) {m_Name = name;}
	~Effect();
	void SetName(const char* name) {m_Name = name;}
	const char* GetName() {return m_Name;}
	bool AddTechnique(std::shared_ptr<Technique> technique);
	bool RemoveTechnique(std::shared_ptr<Technique> technique);
	std::shared_ptr<Technique> GetTechnique(int index);
	std::shared_ptr<Technique> GetTechnique(const char* name);
	int GetNumTechniques() {return (int)m_Techniques.size();} 
};
class Repository
{
private:
	std::vector<std::shared_ptr<PassDesc>>m_Items;
public:
	virtual ~Repository();
	virtual bool AddItem(std::shared_ptr<PassDesc> item);
	virtual bool RemoveItem(const char* name);
	virtual bool RemoveItem(std::shared_ptr<PassDesc> item);
	virtual std::shared_ptr<PassDesc> Find(const char* name);
	virtual std::shared_ptr<PassDesc> Find(unsigned int index);
	virtual unsigned int GetSize() {return (unsigned int)m_Items.size();}
	std::shared_ptr<Repository> Get();
};
