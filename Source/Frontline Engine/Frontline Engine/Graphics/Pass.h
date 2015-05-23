#pragma once
#include "PassDesc.h"

class Technique;
class EffectVisitor;

class Pass
	: public std::enable_shared_from_this<Pass>
{
protected:
	typedef std::vector<std::shared_ptr<PassDesc>> PassDataVec; 
	
	struct Layer 
	{
		Layer() {}
		~Layer() {} 
		PassDataVec dataForValidation;
		PassDataVec dataForExecution;
		std::vector<fxId> storedData; //tells which of the "dataForExecution" is actually stored
	};
	
	std::map<int,Layer> m_Layers;
	
	PassDataVec m_Overrides;
	
	int m_BaseLayer;
	int m_ActiveLayer;
	
	int m_LoopCount;
	
	std::weak_ptr<Technique> m_Parent;
	
	std::map<fxState,std::vector<std::shared_ptr<PassDesc>>> m_PassInfo;
	
	static std::vector<fxState> m_ExecutionConfig;
	
	std::map<std::shared_ptr<Pass>,int> m_OverrideIds;
	
	int m_PassNumber;
	
	bool m_Validated;
	
	unsigned int m_Mask;
	
	const char* m_Name;
	
	std::vector<fxState> m_ExecutionConfigOverrideFlags;
	std::vector<const char*> m_ExecutionConfigOverrideNames;
public:
	Pass(const char* name);
	~Pass();
	
	std::shared_ptr<Pass> Copy() { return nullptr; } //TODO
	
	void SetParent(std::shared_ptr<Technique> parent) {m_Parent = parent;}
	
	void SetName(const char* name) {m_Name = name;}
	const char* GetName() { return m_Name; }
	
	bool IsValid() { return m_Validated; }
	
	void SetFlagActive(fxState flag, bool active);
	bool GetFlagActive(fxState flag);
	
	void SetDataActive(const char* name, bool active);
	bool GetDataActive(const char* name);
	
	void SetPassDataMask(unsigned int mask) { m_Mask = mask; }
	unsigned int GetPassDataMask() const { return m_Mask; }
	
	bool Accept(std::shared_ptr<EffectVisitor> visitor) { return true; } //new, not implemented yet
	
	static void ConfigureExecution(std::vector<fxState> flags) { m_ExecutionConfig = flags; }
	
	void ConfigureExecutionOverride(std::vector<fxState> flags) { m_ExecutionConfigOverrideFlags = flags; } //overrides the static configure execution
	void ConfigureExecuutionOverride(std::vector<const char*> names) { m_ExecutionConfigOverrideNames = names; }

	bool Validate(); 
	bool Execute(); 
	bool Cleanup();
	bool Invalidate();

	void SetActiveLayer(int layer, bool copyFromPrev); 

	//forces validation upon this passdesc and marks it as validated
	bool ForceValidate(std::shared_ptr<PassDesc> data);
	bool ForceValidate(std::vector<std::shared_ptr<PassDesc>>& data) { return true; }

	bool ForceInvalidate(std::shared_ptr<PassDesc> data);
	bool ForceInvalidate(std::vector<std::shared_ptr<PassDesc>>& data) { return true; }

	void CreateData(std::shared_ptr<PassDesc> data); 
	void CreateDataOverride(std::shared_ptr<PassDesc> data); 
	
	//when u dont want to execute the data in this pass or set the pass as its parent
	//useful for allowing elements in this pass access this data, but the actual
	//execution of this data happens in another pass
	void StoreData(std::shared_ptr<PassDesc> data);
	
	void Replace(std::shared_ptr<PassDesc> data);
	void Replace(std::shared_ptr<PassDesc> prevData, std::shared_ptr<PassDesc> newData); //new mothafuka
	void Replace(const char* name, std::shared_ptr<PassDesc> data); 
	void Replace(fxId id, std::shared_ptr<PassDesc> data); //new!!!!
	
	std::shared_ptr<PassDesc> Find(const char* name); //matches up same name
	const std::shared_ptr<PassDesc> Find(const char* name) const; //put const defs for everything, new
	
	std::shared_ptr<PassDesc> Find(fxState flag);
	const std::shared_ptr<PassDesc> Find(fxState flag) const; //new
	
	void Find(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data); //too lazy to const overload the vector
	
	//do the find methods using fxId
	std::shared_ptr<PassDesc> Find(fxId id); //matches up same name
	const std::shared_ptr<PassDesc> Find(fxId id) const; //put const defs for everything, new

	std::shared_ptr<PassDesc> FindOverride(const char* name);
	const std::shared_ptr<PassDesc> FindOverride(const char* name) const; //new
	
	std::shared_ptr<PassDesc> FindOverride(fxState flag);
	const std::shared_ptr<PassDesc> FindOverride(fxState flag) const; //new
	
	void FindOverride(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data);
	
	//do the find methods using fxId
	std::shared_ptr<PassDesc> FindOverride(fxId id); //matches up same name
	const std::shared_ptr<PassDesc> FindOverride(fxId id) const; //put const defs for everything, new
	
	bool ContainsData(std::shared_ptr<PassDesc> data) const; //new, make parameter const?

	//detach the data here
	bool RemoveData(std::shared_ptr<PassDesc> data);
	bool RemoveData(const char* name);
	bool RemoveData(fxState flag);
	bool RemoveData(fxId id); //new!!!!
	
	void ClearData() { m_Layers[m_ActiveLayer].dataForExecution.clear(); }

	void SetupOverrides(std::shared_ptr<Technique>* dest,int numTechs);
	void SetupOverrides(std::shared_ptr<Pass>* dest,int numPasses);
	
	void ReleaseOverrides(std::shared_ptr<Technique>* dest,int numTechs);
	void ReleaseOverrides(std::shared_ptr<Pass>* dest,int numPasses);
	
	bool RemoveLayer(int layer);
	bool RemoveActiveLayer();

	const Layer& GetLayer() const { return m_Layers.find(m_ActiveLayer)->second; }
	const Layer& GetBaseLayer() const { return m_Layers.find(0)->second; }
	
	int GetActiveLayerId() const { return m_ActiveLayer; }
	
	int GetNumPassDesc() const { return (int)m_Layers.find(m_ActiveLayer)->second.dataForExecution.size(); }
	int GetNumPassDescInAllLayers() const; //sums up all the layers' passdesc count
	
	int GetNumLayers() const { return (int)m_Layers.size(); }
	
	void SetLoopCount(int loopCount) { m_LoopCount = loopCount; }
	int GetLoopCount() { return m_LoopCount; }
	
	void SetPassNumber(int pNum) { m_PassNumber = pNum; }
	int GetPassNumber() const { return m_PassNumber; }
	
friend class PassDesc;
};