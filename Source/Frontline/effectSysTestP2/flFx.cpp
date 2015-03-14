#include "flFx.h"

PassDesc::PassDesc(fxState flag,const char* name) {
	m_Flag = flag;
	m_Name = name;
	m_Active = true;
	m_Validated = false;
}
PassDesc::~PassDesc() {
	//delete m_Parent; //handled in technique automatically by shared_ptr
}
//pass stuff
std::vector<fxState> Pass::m_ExecutionConfig;
Pass::Pass(const char* name) {
	m_Name = name;
	m_BaseLayer = 0;
	m_ActiveLayer = 0;
	m_Validated = false;
	SetActiveLayer(0,false);	
}
Pass::~Pass() {
}
bool Pass::Validate() {
	std::cout<<"Calling Validate from Pass at layer: "<<m_ActiveLayer<<std::endl;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) {
		if (!(*iter)->IsValid()) {
			if (!(*iter)->Validate()) {
				continue;
			}
		}
		(*iter)->SetValidated(true);
	}
	m_Validated = true;
	return true;
}
bool Pass::Execute() {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	m_PassInfo.clear();
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) {
		m_PassInfo[(*iter)->m_Flag].push_back(*iter);
	}
	if (!Pass::m_ExecutionConfig.empty()) {
		for (auto iter = Pass::m_ExecutionConfig.begin(); iter != Pass::m_ExecutionConfig.end(); iter++) {
			fxState current = *iter;
			for (auto iter = m_PassInfo[current].begin(); iter != m_PassInfo[current].end(); iter++) {
				if ((*iter)->IsActive() && (*iter)->IsValid()) {
					(*iter)->Execute();
				}
			}
		}
	}
	return true;
}
//cleanup is called manually
bool Pass::Cleanup() {
	bool success = true;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) {
		if ((*iter)->IsActive()) {
			if (!(*iter)->Cleanup()) {
				success = false;
			}
		}
	}
	return success;
}
bool Pass::Invalidate() {
	std::cout<<"Calling Invalidate from Pass at layer: "<<m_ActiveLayer<<std::endl;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) {
		if ((*iter)->IsValid()) {
			if (!(*iter)->Invalidate()) {
				continue;
			}
		}
		(*iter)->SetValidated(false);
	}
	m_Validated = false;
	return true;
}
void Pass::SetActiveLayer(int layer, bool copyFromPrev) {
	if(layer < 0 || layer == m_ActiveLayer) {
		return;
	}
	//check if layer exists, if does, then set that to active
	for(auto iter = m_Layers.begin(); iter != m_Layers.end(); iter++) {
		if(iter->first == layer) {
			m_ActiveLayer = iter->first;
			return;
		}
	}
	//if layer doesnt exist, create new layer
	Layer newLayer;
	if(copyFromPrev) {
		//copies from prev active layer
		newLayer = m_Layers[m_ActiveLayer]; //all operations on top layer should reflect all layers
		//newLayer.Copy(m_Layers[m_ActiveLayer]);
	}
	//Invalidate(); //maybe invalidate previous layer?
	//m_Validated = false; //since new layer, maybe umm set validated to false?
	m_Layers[layer] = newLayer;
	m_ActiveLayer = layer;
	std::cout<<"Set ActiveLayer to: "<<m_ActiveLayer<<std::endl;
}
void Pass::CreateData(std::shared_ptr<PassDesc> data) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) {
		if((*iter)->m_Name && data->m_Name) {
			if(strcmp((*iter)->m_Name,data->m_Name) == 0) {
				return;
			}
		}
		if(data == (*iter)) {
			return;
		}
	}
	data->SetParent(shared_from_this()); //this line >:(
	if(m_Validated) { //if(m_Validated) //if the pass was already validated, validate here then for consistency
		data->Validate();
		data->m_Validated = true; //set flag here too
	}
	m_Layers[m_ActiveLayer].dataForExecution.push_back(data);
}
void Pass::CreateDataOverride(std::shared_ptr<PassDesc> data) {
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end(); iter++) {
		if((*iter)->m_Name && data->m_Name) {
			if(strcmp((*iter)->m_Name,data->m_Name) == 0) {
				return;
			}
		}
		if(data == (*iter)) {
			return;
		}
	}
	//data->SetParent(shared_from_this());
	//validation and set parent not needed cuz it relies on the pass it overrides
	m_Overrides.push_back(data);
}
void Pass::Replace(std::shared_ptr<PassDesc> data) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if ((*iter)->m_Name && data->m_Name) {
			if (strcmp((*iter)->m_Name,data->m_Name) == 0) {
				data->SetParent(shared_from_this());
				if (m_Validated) {
					data->Validate();
					data->m_Validated = true; 
				}
				(*iter) = data;
				return;
			}
		}
		iter++;
	}
}
void Pass::Replace(const char* name, std::shared_ptr<PassDesc> data) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if ((*iter)->m_Name && name) {
			if (strcmp(name,(*iter)->m_Name) == 0) {
				data->SetParent(shared_from_this());
				if (m_Validated) {
					data->Validate();
					data->m_Validated = true;
				}
				(*iter) = data;
				return;
			}
		}
		iter++;
	}
}
std::shared_ptr<PassDesc> Pass::Find(const char* name) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if((*iter)->m_Name) {
			if(strcmp((*iter)->m_Name,name) == 0) { //oh thats why, violation was in strcmp cuz of null string
				return (*iter); //wtf access violation??!
			}
		}
		iter++;
	}
	return NULL;
}
std::shared_ptr<PassDesc> Pass::Find(fxState flag) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if((*iter)->m_Flag == flag) {
			return (*iter);
		}
		iter++;
	}
	return NULL;
}
void Pass::Find(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data) {
	std::vector<std::shared_ptr<PassDesc>> sortedData;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) {
		if ((*iter)->GetFlag() == flag) {
			sortedData.push_back(*iter);
		}
	}
	data = &sortedData;
}
std::shared_ptr<PassDesc> Pass::FindOverride(const char* name) {
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) {
		if((*iter)->m_Name) {
			if(strcmp((*iter)->m_Name,name) == 0) {
				return (*iter);
			}
		}
		iter++;
	}
	return NULL;
}
std::shared_ptr<PassDesc> Pass::FindOverride(fxState flag) {
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) {
		if((*iter)->m_Flag == flag) {
			return (*iter);
		}
		iter++;
	}
	return NULL;
}
void Pass::FindOverride(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data) {
	std::vector<std::shared_ptr<PassDesc>> sortedData;
	for (auto iter = m_Overrides.begin(); iter != m_Overrides.end(); iter++) {
		if ((*iter)->GetFlag() == flag) {
			sortedData.push_back(*iter);
		}
	}
	data = &sortedData;
}
bool Pass::RemoveData(std::shared_ptr<PassDesc> data) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if((*iter) == data) {
			(*iter)->Invalidate();
			(*iter)->m_Validated = false;
			dataForExecution.erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}
bool Pass::RemoveData(const char* name) {
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if((*iter)->m_Name) {
			if(strcmp((*iter)->m_Name,name) == 0) {
				(*iter)->Invalidate();
				(*iter)->m_Validated = false;
				dataForExecution.erase(iter);
				return true;
			}
		}
		iter++;
	}
	return false;
}
bool Pass::RemoveData(fxState flag) {
	bool removed = false;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) {
		if((*iter)->m_Flag == flag) {
			(*iter)->Invalidate();
			(*iter)->m_Validated = false;
			dataForExecution.erase(iter);
			removed = true;
		}
		iter++;
	}
	return removed;
}
void Pass::SetupOverrides(std::shared_ptr<Technique>* dest, int numTechs) {
	for(int i = 0; i < numTechs; i++) {
		std::shared_ptr<Technique> destTech = dest[i];
		for(int i = 0; i < destTech->GetNumPasses(); i++) {
			std::shared_ptr<Pass> pass = destTech->GetPass(i);
			SetupOverrides(&pass,1);
		}
	}
}
//program is bound on the active layer
void Pass::SetupOverrides(std::shared_ptr<Pass>* dest, int numPasses) {
	for(int i = 0; i < numPasses; i++) {
		std::shared_ptr<Pass> passDst = dest[i];
		if(m_Overrides.size() > 0) {
			int layer = passDst->GetActiveLayerId();
			layer++;
			m_OverrideIds[passDst] = layer;
			passDst->Invalidate(); //can be handled in SetActiveLayer
			passDst->SetActiveLayer(m_OverrideIds[passDst],true); //create new layer
			for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();iter++) {
				//if names match up,usually names are multiples, invalidated already??
				bool removed = passDst->RemoveData((*iter)->m_Name); //remove and replace
				if(removed) {
					std::cout<<"Overrided "<<(*iter)->m_Name<<" in Pass "<<passDst->GetName()<<std::endl;
					passDst->CreateData((*iter));
				}
				//passDst->Replace(*iter); //to save locations
			}
			//passDst->Validate(); //validate new layer
		}
	}
}
void Pass::ReleaseOverrides(std::shared_ptr<Technique>* dest, int numTechniques) {
	for(int i = 0; i < numTechniques; i++) {
		std::shared_ptr<Technique> destTech = dest[i];
		for(int i = 0; i < destTech->GetNumPasses(); i++) {
			std::shared_ptr<Pass> pass = destTech->GetPass(i);
			ReleaseOverrides(&pass,1);
		}
	}
}
//usually, SetActiveLayer() to designated layer will be called before ReleaseingOverrides to avoid setting active layer to 0
void Pass::ReleaseOverrides(std::shared_ptr<Pass>* dest, int numPasses) {
	for(int i = 0; i < numPasses; i++) {
		std::shared_ptr<Pass> passDst = dest[i];
		passDst->RemoveLayer(m_OverrideIds[passDst]);
		//passDst->Validate(); //validate the layer that is now uncovered
		m_OverrideIds.erase(passDst);
	}
}
bool Pass::RemoveLayer(int layer) {
	m_Layers.erase(layer);
	if(layer == m_ActiveLayer) {
		std::cout<<"Warning, removing active layer, resetting active layer"<<std::endl;
		SetActiveLayer(0,false);
	}
	return true;
}
Technique::~Technique() {
}
bool Technique::Validate() {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(!iter->pass->Validate()) {
			return false;
		}
	}
	return true;
}
bool Technique::Invalidate() {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(!iter->pass->Invalidate()) {
			return false;
		}
	}
	return true;
}
bool Technique::AddPass(std::shared_ptr<Pass> pass) {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(iter->pass == pass) {
			return false;
		}
	}
	PassInfo passInfo;
	passInfo.isActive = true;
	pass->SetParent(shared_from_this());
	passInfo.pass = pass;
	m_Passes.push_back(passInfo);
	return true;
}
bool Technique::RemovePass(std::shared_ptr<Pass> pass) {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(iter->pass == pass) {
			m_Passes.erase(iter);
			return true;
		}
	}
	return false;
}
void Technique::SetPassActive(int index, bool active) {
	if(index >= m_Passes.size()) {
		return;
	}
	m_Passes[index].isActive = active;
}
void Technique::SetPassActive(const char* name, bool active) {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(strcmp(iter->pass->GetName(),name) == 0) {
			iter->isActive = active;
			break;
		}
	}
}
bool Technique::GetPassActive(int index) {
	if(index >= m_Passes.size()) {
		return false;
	}
	return m_Passes[index].isActive;
}
bool Technique::GetPassActive(const char* name) {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(strcmp(iter->pass->GetName(),name) == 0) {
			return iter->isActive;
		}
	}
	return false;
}
std::shared_ptr<Pass> Technique::GetPass(int index) {
	if(index >= m_Passes.size()) {
		return NULL;
	}
	return std::shared_ptr<Pass>(m_Passes[index].pass);
}
std::shared_ptr<Pass> Technique::GetPass(const char* name) {
	for(auto iter = m_Passes.begin(); iter != m_Passes.end(); iter++) {
		if(strcmp(iter->pass->GetName(),name) == 0) {
			return iter->pass;
		}
	}
	return NULL;
}
Effect::~Effect() {
}
bool Effect::AddTechnique(std::shared_ptr<Technique> technique) {
	for(auto iter = m_Techniques.begin(); iter != m_Techniques.end(); iter++) {
		if((*iter) == technique) {
			return false;
		}
	}
	m_Techniques.push_back(technique);
	return true;
}
bool Effect::RemoveTechnique(std::shared_ptr<Technique> technique) {
	for(auto iter = m_Techniques.begin(); iter != m_Techniques.end(); iter++) {
		if((*iter) == technique) {
			m_Techniques.erase(iter);
			return true;
		}
	}
	return false;
}
std::shared_ptr<Technique> Effect::GetTechnique(int index) {
	if(index >= m_Techniques.size()) {
		return NULL;
	}
	return std::shared_ptr<Technique>(m_Techniques[index]);
}
std::shared_ptr<Technique> Effect::GetTechnique(const char* name) {
	for(auto iter = m_Techniques.begin(); iter != m_Techniques.end(); iter++) {
		if(strcmp((*iter)->GetName(),name) == 0) {
			return (*iter);
		}
	}
	return NULL;
}
Repository::~Repository() {
}
bool Repository::AddItem(std::shared_ptr<PassDesc> item) {
	for(auto iter = m_Items.begin(); iter != m_Items.end(); iter++) {
		if((*iter) == item) {
			return false;
		}
	}
	m_Items.push_back(item);
	return true;
}
bool Repository::RemoveItem(const char* name) {
	for(auto iter = m_Items.begin(); iter != m_Items.end();) {
		if(strcmp((*iter)->GetName(),name) == 0) {
			m_Items.erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}
bool Repository::RemoveItem(std::shared_ptr<PassDesc> item) {
	for(auto iter = m_Items.begin(); iter != m_Items.end();) {
		if((*iter) == item) {
			m_Items.erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}
std::shared_ptr<PassDesc> Repository::Find(const char* name) {
	for(auto iter = m_Items.begin(); iter != m_Items.end();) {
		if(strcmp((*iter)->GetName(),name) == 0) {
			return (*iter);
		}
		iter++;
	}
	return NULL;
}
std::shared_ptr<PassDesc> Repository::Find(unsigned int index) {
	if(index >= m_Items.size()) {
		return NULL;
	}
	return m_Items[index];
}
