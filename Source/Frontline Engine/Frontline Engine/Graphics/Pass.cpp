#include "Pass.h"
#include "Technique.h"
#include "PassDesc.h"
#include "Effect.h"
#include "Pass.h"

std::vector<fxState> Pass::m_ExecutionConfig;

Pass::Pass(const char* name) 
{
	m_Name = name;
	m_BaseLayer = 0;
	m_ActiveLayer = 0;
	m_Validated = false;
	SetActiveLayer(0,false);	
}

Pass::~Pass() 
{
}

bool Pass::Validate() 
{
	std::cout<<"Calling Validate from Pass at layer: "<<m_ActiveLayer<<std::endl;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	auto& storedDataKeys = m_Layers[m_ActiveLayer].storedData;
	
	//validate attachments fist
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		if (!(*iter)->IsValid()) 
		{
			(*iter)->ValidateAttachments(shared_from_this());
			//(*iter)->ValidateAttachments(shared_from_this());
		}
	}
	
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		bool stored = false;
		for (auto key = storedDataKeys.begin(); key != storedDataKeys.end(); key++)
		{
			if ((*iter)->GetId() == (*key))
			{
				stored = true;
				break;
			}
		}
		if (stored)
		{
			continue;
		}
		if (!(*iter)->IsValid()) 
		{
			if (!(*iter)->Validate()) 
			{
				continue;
			}
		}
		(*iter)->SetValidated(true);
	}
	m_Validated = true;
	return true;
}

bool Pass::Execute() 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	auto& storedDataKeys = m_Layers[m_ActiveLayer].storedData;

	m_PassInfo.clear();

	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		//if its stored data, dont add it to the list of data that is to be executed
		bool stored = false;
		for (auto key = storedDataKeys.begin(); key != storedDataKeys.end(); key++)
		{
			if ((*iter)->GetId() == (*key))
			{
				stored = true;
				break;
			}
		}
		if (stored)
		{
			continue;
		}
		m_PassInfo[(*iter)->m_Flag].push_back(*iter);
	}
	if (!Pass::m_ExecutionConfig.empty()) 
	{
		for (auto iter = Pass::m_ExecutionConfig.begin(); iter != Pass::m_ExecutionConfig.end(); iter++) 
		{
			fxState current = *iter;
			for (auto iter = m_PassInfo[current].begin(); iter != m_PassInfo[current].end(); iter++) 
			{
				if ((*iter)->IsActive() && (*iter)->IsValid()) 
				{
					(*iter)->Execute();
				}
			}
		}
	}
	return true;
}

//cleanup is called manually
bool Pass::Cleanup() 
{
	bool success = true;

	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	auto& storedDataKeys = m_Layers[m_ActiveLayer].storedData;

	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		//if it didnt execute, it doesnt need to cleanup
		bool stored = false;
		for (auto key = storedDataKeys.begin(); key != storedDataKeys.end(); key++)
		{
			if ((*iter)->GetId() == (*key))
			{
				stored = true;
				break;
			}
		}
		if (stored)
		{
			continue;
		}
		if ((*iter)->IsActive()) 
		{
			if (!(*iter)->Cleanup()) 
			{
				success = false;
			}
		}
	}
	return success;
}

bool Pass::Invalidate() 
{
	std::cout<<"Calling Invalidate from Pass at layer: "<<m_ActiveLayer<<std::endl;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	auto& storedDataKeys = m_Layers[m_ActiveLayer].storedData;
	
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		if ((*iter)->IsValid()) 
		{
			if (!(*iter)->Invalidate()) 
			{
				continue;
			}
		}
		(*iter)->SetValidated(false);
	}
	
	//invalidate attachments after cuz data invalidation may rely on attachments
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		bool stored = false;
		for (auto key = storedDataKeys.begin(); key != storedDataKeys.end(); key++)
		{
			if ((*iter)->GetId() == (*key))
			{
				stored = true;
				break;
			}
		}
		if (stored)
		{
			continue;
		}
		if ((*iter)->IsValid()) 
		{
			(*iter)->InvalidateAttachments(shared_from_this());
		}
	}
	
	m_Validated = false;
	return true;
}

void Pass::SetActiveLayer(int layer, bool copyFromPrev) 
{
	if(layer < 0 || layer == m_ActiveLayer) 
	{
		return;
	}
	//check if layer exists, if does, then set that to active
	for(auto iter = m_Layers.begin(); iter != m_Layers.end(); iter++) 
	{
		if(iter->first == layer) 
		{
			m_ActiveLayer = iter->first;
			return;
		}
	}
	
	//if layer doesnt exist, create new layer
	Layer newLayer;
	if(copyFromPrev) 
	{
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

bool Pass::ForceValidate(std::shared_ptr<PassDesc> data)
{
	bool res = false;
	if (data->IsValid())
		return false;
	res = data->Validate();
	data->SetValidated(true);
	return res;
}

bool Pass::ForceInvalidate(std::shared_ptr<PassDesc> data)
{
	bool res = false;
	if (!data->IsValid())
		return false;
	res = data->Invalidate();
	data->SetValidated(false);
	return res;
}

void Pass::CreateData(std::shared_ptr<PassDesc> data) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		if((*iter)->m_Name && data->m_Name) 
		{
			if(strcmp((*iter)->m_Name,data->m_Name) == 0) 
			{
				return;
			}
		}
		if(data == (*iter)) 
		{
			return;
		}
	}
	
	data->SetParent(shared_from_this()); //this line >:(
	
	//warning, creating data after Pass::Validate may cause data dependency problems
	if(m_Validated) 
	{ //if(m_Validated) //if the pass was already validated, validate here then for consistency
		std::cout<<"Warning! Validating data after Pass::Validate() may create data dependency problems!"<<std::endl;
		data->ValidateAttachments(shared_from_this());
		data->Validate();
		data->m_Validated = true; //set flag here too
	}
	m_Layers[m_ActiveLayer].dataForExecution.push_back(data);
}

void Pass::CreateDataOverride(std::shared_ptr<PassDesc> data) 
{
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end(); iter++) 
	{
		if((*iter)->m_Name && data->m_Name) 
		{
			if(strcmp((*iter)->m_Name,data->m_Name) == 0) 
			{
				return;
			}
		}
		if(data == (*iter)) 
		{
			return;
		}
	}
	
	//data->SetParent(shared_from_this());
	//validation and set parent not needed cuz it relies on the pass it overrides
	m_Overrides.push_back(data);
}

void Pass::StoreData(std::shared_ptr<PassDesc> data)
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		if((*iter)->m_Name && data->m_Name) 
		{
			if(strcmp((*iter)->m_Name,data->m_Name) == 0) 
			{
				return;
			}
		}
		if(data == (*iter)) 
		{
			return;
		}
	}
	//just store data, dont do anything with it
	m_Layers[m_ActiveLayer].dataForExecution.push_back(data);
	m_Layers[m_ActiveLayer].storedData.push_back(data->GetId());
}

void Pass::Replace(std::shared_ptr<PassDesc> data) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if ((*iter)->m_Name && data->m_Name) 
		{
			if (strcmp((*iter)->m_Name,data->m_Name) == 0) 
			{		
				data->SetParent(shared_from_this());
				if (m_Validated) 
				{
					(*iter)->Invalidate();
					(*iter)->InvalidateAttachments(shared_from_this());
					data->ValidateAttachments(shared_from_this());
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

void Pass::Replace(std::shared_ptr<PassDesc> prevData, std::shared_ptr<PassDesc> newData)
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if ((*iter) == prevData) 
		{
			newData->SetParent(shared_from_this());
			if (m_Validated) 
			{
				(*iter)->InvalidateAttachments(shared_from_this());
				(*iter)->Invalidate();
				newData->ValidateAttachments(shared_from_this());
				newData->Validate();
				newData->m_Validated = true; 
			}
			(*iter) = newData;
			return;
		}
		iter++;
	}
}

void Pass::Replace(const char* name, std::shared_ptr<PassDesc> data) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if ((*iter)->m_Name && name) 
		{
			if (strcmp(name,(*iter)->m_Name) == 0) 
			{
				data->SetParent(shared_from_this());
				if (m_Validated) 
				{
					(*iter)->Invalidate();
					(*iter)->InvalidateAttachments(shared_from_this());
					data->ValidateAttachments(shared_from_this());
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

void Pass::Replace(fxId id, std::shared_ptr<PassDesc> data) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if ((*iter)->GetId() == id) 
		{
			data->SetParent(shared_from_this());
			if (m_Validated) 
			{
				(*iter)->Invalidate();
				(*iter)->InvalidateAttachments(shared_from_this());
				data->ValidateAttachments(shared_from_this());
				data->Validate();
				data->m_Validated = true;
			}
			(*iter) = data;
			return;
		}
		iter++;
	}
}

std::shared_ptr<PassDesc> Pass::Find(const char* name) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->m_Name) 
		{
			if(strcmp((*iter)->m_Name,name) == 0) 
			{ //oh thats why, violation was in strcmp cuz of null string
				return (*iter); //wtf access violation??!
			}
		}
		iter++;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> Pass::Find(const char* name) const 
{
	//but protection... wut if data doesnt exist
	const PassDataVec& dataForExecution = m_Layers.find(m_ActiveLayer)->second.dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->m_Name) 
		{
			if(strcmp((*iter)->m_Name,name) == 0) 
			{ //oh thats why, violation was in strcmp cuz of null string
				return (*iter); //wtf access violation??!
			}
		}
		iter++;
	}
	return NULL;
}

std::shared_ptr<PassDesc> Pass::Find(fxState flag) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->m_Flag == flag) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> Pass::Find(fxState flag) const
{
	const PassDataVec& dataForExecution = m_Layers.find(m_ActiveLayer)->second.dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->m_Flag == flag) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

void Pass::Find(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data)
{
	std::vector<std::shared_ptr<PassDesc>> sortedData;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for (auto iter = dataForExecution.begin(); iter != dataForExecution.end(); iter++) 
	{
		if ((*iter)->GetFlag() == flag) 
		{
			sortedData.push_back(*iter);
		}
	}
	data = &sortedData;
}

std::shared_ptr<PassDesc> Pass::Find(fxId id)
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->GetId() == id) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> Pass::Find(fxId id) const
{
	const PassDataVec& dataForExecution = m_Layers.find(m_ActiveLayer)->second.dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->GetId() == id) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

std::shared_ptr<PassDesc> Pass::FindOverride(const char* name) 
{
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) 
	{
		if((*iter)->m_Name) 
		{
			if(strcmp((*iter)->m_Name,name) == 0) 
			{
				return (*iter);
			}
		}
		iter++;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> Pass::FindOverride(const char* name) const
{
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) 
	{
		if((*iter)->m_Name) 
		{
			if(strcmp((*iter)->m_Name,name) == 0) 
			{
				return (*iter);
			}
		}
		iter++;
	}
	return NULL;
}

std::shared_ptr<PassDesc> Pass::FindOverride(fxState flag) 
{
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) 
	{
		if((*iter)->m_Flag == flag) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> Pass::FindOverride(fxState flag) const
{
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) 
	{
		if((*iter)->m_Flag == flag) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

void Pass::FindOverride(fxState flag, std::vector<std::shared_ptr<PassDesc>>* data)
{
	std::vector<std::shared_ptr<PassDesc>> sortedData;
	for (auto iter = m_Overrides.begin(); iter != m_Overrides.end(); iter++) 
	{
		if ((*iter)->GetFlag() == flag) 
		{
			sortedData.push_back(*iter);
		}
	}
	data = &sortedData;
}

std::shared_ptr<PassDesc> Pass::FindOverride(fxId id)
{
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) 
	{
		if((*iter)->GetId() == id) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> Pass::FindOverride(fxId id) const
{
	//use std::vector::cbegin()?
	for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();) 
	{
		if((*iter)->GetId() == id) 
		{
			return (*iter);
		}
		iter++;
	}
	return NULL;
}

bool Pass::ContainsData(std::shared_ptr<PassDesc> data) const
{
	const PassDataVec& dataForExecution = m_Layers.find(m_ActiveLayer)->second.dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter) == data)
			return true;
	}
	return false;
}

bool Pass::RemoveData(std::shared_ptr<PassDesc> data) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter) == data) 
		{
			(*iter)->Invalidate();
			//invalidate attachments last always
			(*iter)->InvalidateAttachments(shared_from_this());
			(*iter)->m_Validated = false;
			dataForExecution.erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}

bool Pass::RemoveData(const char* name) 
{
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->m_Name) 
		{
			if(strcmp((*iter)->m_Name,name) == 0) {
				(*iter)->Invalidate();
				(*iter)->InvalidateAttachments(shared_from_this());
				(*iter)->m_Validated = false;
				dataForExecution.erase(iter);
				return true;
			}
		}
		iter++;
	}
	return false;
}

bool Pass::RemoveData(fxState flag) 
{
	bool removed = false;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->m_Flag == flag) 
		{
			(*iter)->Invalidate();
			(*iter)->InvalidateAttachments(shared_from_this());
			(*iter)->m_Validated = false;
			dataForExecution.erase(iter);
			removed = true;
		}
		iter++;
	}
	return removed;
}

bool Pass::RemoveData(fxId id) 
{
	bool removed = false;
	PassDataVec& dataForExecution = m_Layers[m_ActiveLayer].dataForExecution;
	for(auto iter = dataForExecution.begin(); iter != dataForExecution.end();) 
	{
		if((*iter)->GetId() == id) 
		{
			(*iter)->Invalidate();
			(*iter)->InvalidateAttachments(shared_from_this());
			(*iter)->m_Validated = false;
			dataForExecution.erase(iter);
			removed = true;
		}
		iter++;
	}
	return removed;
}

void Pass::SetupOverrides(std::shared_ptr<Technique>* dest, int numTechs) 
{
	for(int i = 0; i < numTechs; i++) 
	{
		std::shared_ptr<Technique> destTech = dest[i];
		for(int i = 0; i < destTech->GetNumPasses(); i++) 
		{
			std::shared_ptr<Pass> pass = destTech->GetPass(i);
			SetupOverrides(&pass,1);
		}
	}
}

//program is bound on the active layer
void Pass::SetupOverrides(std::shared_ptr<Pass>* dest, int numPasses) 
{
	for(int i = 0; i < numPasses; i++) 
	{
		std::shared_ptr<Pass> passDst = dest[i];
		if(m_Overrides.size() > 0) 
		{
			int layer = passDst->GetActiveLayerId();
			layer++;
			m_OverrideIds[passDst] = layer;
			passDst->Invalidate(); //can be handled in SetActiveLayer
			passDst->SetActiveLayer(m_OverrideIds[passDst],true); //create new layer
			for(auto iter = m_Overrides.begin(); iter != m_Overrides.end();iter++) 
			{
				//if names match up,usually names are multiples, invalidated already??
				bool removed = passDst->RemoveData((*iter)->m_Name); //remove and replace
				if(removed) 
				{
					std::cout<<"Overrided "<<(*iter)->m_Name<<" in Pass "<<passDst->GetName()<<std::endl;
					passDst->CreateData((*iter));
				}
				//passDst->Replace(*iter); //to save locations
			}
			//passDst->Validate(); //validate new layer
		}
	}
}

void Pass::ReleaseOverrides(std::shared_ptr<Technique>* dest, int numTechniques) 
{
	for(int i = 0; i < numTechniques; i++) 
	{
		std::shared_ptr<Technique> destTech = dest[i];
		for(int i = 0; i < destTech->GetNumPasses(); i++) 
		{
			std::shared_ptr<Pass> pass = destTech->GetPass(i);
			ReleaseOverrides(&pass,1);
		}
	}
}

//usually, SetActiveLayer() to designated layer will be called before ReleaseingOverrides to avoid setting active layer to 0
void Pass::ReleaseOverrides(std::shared_ptr<Pass>* dest, int numPasses) 
{
	for(int i = 0; i < numPasses; i++) 
	{
		std::shared_ptr<Pass> passDst = dest[i];
		passDst->RemoveLayer(m_OverrideIds[passDst]);
		//passDst->Validate(); //validate the layer that is now uncovered
		m_OverrideIds.erase(passDst);
	}
}

bool Pass::RemoveLayer(int layer) 
{
	m_Layers.erase(layer);
	if(layer == m_ActiveLayer) 
	{
		std::cout<<"Warning, removing active layer, resetting active layer"<<std::endl;
		SetActiveLayer(0,false);
	}
	return true;
}

int Pass::GetNumPassDescInAllLayers() const
{
	int count = 0;
	for(auto iter = m_Layers.begin(); iter != m_Layers.end(); iter++)
	{
		count += iter->second.dataForExecution.size();
	}
	return count;
}