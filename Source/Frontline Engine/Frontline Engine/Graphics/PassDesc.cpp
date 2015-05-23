#include "PassDesc.h"
#include "Pass.h"

fxId PassDesc::m_IdCounter = 0;

PassDesc::PassDesc(fxState flag,const char* name) 
{
	m_Flag = flag;
	m_Name = name;
	m_Active = true;
	m_Validated = false;
	m_IsInPass = false;
	m_Id = PassDesc::GenerateId();
}

fxId PassDesc::GenerateId()
{
	m_IdCounter+=1;
	return m_IdCounter;
}

PassDesc::~PassDesc() 
{
	//detach the children
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		//all internal operations using Id
		PassDescInfo* info = (*iter);
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if(!data)
		{
			//if there is no data, lets assume this passDesc has already been detached from
			//the parent and all the shit has been dealt with
			continue;
		}
			
		//remove this data from the owners list of its children data	
		for (auto dataIter = data->m_Owners.begin(); dataIter != data->m_Owners.end();)
		{
			if((*dataIter)->id == m_Id)
			{
				//remove
				data->m_Owners.erase(dataIter);
				break;
			}
			else
			{
				dataIter++;
			}
		}
	}
	m_Data.clear();
	
	//detach from parent
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		std::shared_ptr<PassDesc> owner = info->container.lock()->Find(info->id); //find it from its own pass
		if(!owner)
		{
			continue;
		}
		
		//have the parent detach this from its data
		for (auto ownerIter = owner->m_Data.begin(); ownerIter != owner->m_Data.end();)
		{
			if ((*ownerIter)->id == m_Id)
			{
				//remove
				owner->m_Data.erase(ownerIter);
				break;
			}
			else
			{
				ownerIter++;
			}
		}
	}
	m_Owners.clear();
}

//////////////////////////////////////////////////////////////////////////////////////

bool PassDesc::Attach(std::shared_ptr<PassDesc> data)
{
	if (!m_Parent.lock())
	{
		return false;
	}
	if (!data->IsInPass())
	{
		std::cout << "Pass data must be in pass before use" << std::endl;
		return false;
	}
	//make sure this data isint already existing here
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		if ((*iter)->id == data->GetId())
			return false;
	}

	PassDescInfo* info = new PassDescInfo;
	info->container = data->GetParent();
	info->id = data->GetId();

	m_Data.push_back(info);

	PassDescInfo* owner = new PassDescInfo;
	owner->container = m_Parent.lock();
	owner->id = m_Id;

	data->m_Owners.push_back(owner);
	return true;
}

bool PassDesc::Attach(const char* name) //remove the flag parameter
{
	//if container isint specified, default to this parent
	if (m_Parent.lock())
	{
		std::shared_ptr<PassDesc> data = m_Parent.lock()->Find(name);
		if(!data)
			return false;
		return Attach(data);
	}
	return false;
}


//make one for the Id
bool PassDesc::Attach(fxId id)
{
	if(m_Parent.lock())
	{
		std::shared_ptr<PassDesc> data = m_Parent.lock()->Find(id);
		if(!data)
			return false;
		return Attach(data);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////

bool PassDesc::Attach(std::shared_ptr<Pass> container, const char* name) //remove the flag parameter
{
	//if container isint specified, default to this parent
	if (container)
	{
		std::shared_ptr<PassDesc> data = container->Find(name);
		if (!data)
			return false;
		return Attach(data);
	}
	return false;
}


//make one for the Id
bool PassDesc::Attach(std::shared_ptr<Pass> container, fxId id)
{
	if (container)
	{
		std::shared_ptr<PassDesc> data = container->Find(id);
		if (!data)
			return false;
		return Attach(data);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////

bool PassDesc::Detach(std::shared_ptr<PassDesc> data)
{
	bool erased = false;
	for (auto iter = m_Data.begin(); iter != m_Data.end();)
	{
		if ((*iter)->id == data->GetId())
		{
			//remove
			erased = true;
			m_Data.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}

	//now do the owners
	for (auto iter = data->m_Owners.begin(); iter != data->m_Owners.end();)
	{
		if ((*iter)->id == m_Id)
		{
			//remove
			data->m_Owners.erase(iter);
			break;
		}
		else
		{
			iter++;
		}
	}
	return erased;
}

bool PassDesc::Detach(const char* name)
{	
	std::shared_ptr<PassDesc> data = FindData(name);
	if(!data)
		return false;
		
	bool res = Detach(data);
	return res;
}

bool PassDesc::Detach(fxId id)
{	
	std::shared_ptr<PassDesc> data = FindData(id);
	if(!data)
		return false;
		
	bool res = Detach(data);
	return res;
}

//////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<PassDesc> PassDesc::FindData(const char* name)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetName()) 
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindData(const char* name) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		//PassDescInfo* info = (*iter);
		//its ok if this and the other data have the same parent
		//std::shared_ptr<PassDesc> data = info->parent.lock()->Find(info->id);
		//if(data) ...
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetName()) 
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindData(std::shared_ptr<Pass> container, const char* name)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetName())
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindData(std::shared_ptr<Pass> container, const char* name) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetName())
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindData(fxState flag)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindData(fxState flag) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindData(std::shared_ptr<Pass> container, fxState flag)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindData(std::shared_ptr<Pass> container, fxState flag) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindData(fxId id)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		if ((*iter)->id == id)
		{
			PassDescInfo* info = (*iter);
			if (!info->container.lock())
			{
				continue;
			}
			std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
			if(!data)
				continue;
			return data;
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindData(fxId id) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		if ((*iter)->id == id)
		{
			PassDescInfo* info = (*iter);
			if (!info->container.lock())
			{
				continue;
			}
			std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
			if(!data)
				continue;
			return data;
		}
	}
	return NULL;
}

void PassDesc::FindData(fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

void PassDesc::FindData(fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

void PassDesc::FindData(std::shared_ptr<Pass> container, fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec)
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

void PassDesc::FindData(std::shared_ptr<Pass> container, fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const
{
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

std::shared_ptr<PassDesc> PassDesc::FindData(int index)
{
	if (index < m_Data.size())
	{
		PassDescInfo* info = m_Data.at(index);
		if (!info->container.lock())
		{
			return NULL;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		return data;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindData(int index) const
{
	if (index < m_Data.size())
	{
		PassDescInfo* info = m_Data.at(index);
		if (!info->container.lock())
		{
			return NULL;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		return data;
	}
	return NULL;
}

/**********************************************************************/

std::shared_ptr<PassDesc> PassDesc::FindOwner(const char* name)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetName()) 
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindOwner(const char* name) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetName()) 
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindOwner(std::shared_ptr<Pass> container, const char* name)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetName())
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindOwner(std::shared_ptr<Pass> container, const char* name) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetName())
		{	//			iter->name
			if (strcmp(data->GetName(), name) == 0)
			{
				return data;
			}
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindOwner(fxState flag)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindOwner(fxState flag) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindOwner(std::shared_ptr<Pass> container, fxState flag)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindOwner(std::shared_ptr<Pass> container, fxState flag) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			return data;
		}
	}
	return NULL;
}

std::shared_ptr<PassDesc> PassDesc::FindOwner(fxId id)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		if ((*iter)->id == id)
		{
			PassDescInfo* info = (*iter);
			if (!info->container.lock())
			{
				continue;
			}
			std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
			if(!data)
				continue;
			return data;
		}
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindOwner(fxId id) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		if ((*iter)->id == id)
		{
			PassDescInfo* info = (*iter);
			if (!info->container.lock())
			{
				continue;
			}
			std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
			if(!data)
				continue;
			return data;
		}
	}
	return NULL;
}

void PassDesc::FindOwner(fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

void PassDesc::FindOwner(fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

void PassDesc::FindOwner(std::shared_ptr<Pass> container, fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec)
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

void PassDesc::FindOwner(std::shared_ptr<Pass> container, fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const
{
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		std::shared_ptr<PassDesc> data = container->Find((*iter)->id); //find it from its own pass
		if (data->GetFlag() == flag)
		{
			dataVec.push_back(data);
		}
	}
	return;
}

std::shared_ptr<PassDesc> PassDesc::FindOwner(int index)
{
	if (index < m_Owners.size())
	{
		PassDescInfo* info = m_Owners.at(index);
		if (!info->container.lock())
		{
			return NULL;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		return data;
	}
	return NULL;
}

const std::shared_ptr<PassDesc> PassDesc::FindOwner(int index) const
{
	if (index < m_Owners.size())
	{
		PassDescInfo* info = m_Owners.at(index);
		if (!info->container.lock())
		{
			return NULL;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		return data;
	}
	return NULL;
}

/*********************************************************************************/

std::vector<std::shared_ptr<PassDesc>> PassDesc::GetVecFromData()
{	
	std::vector<std::shared_ptr<PassDesc>> dataVec;
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data)
			dataVec.push_back(data);
	}
	return dataVec;
}

std::vector<const std::shared_ptr<PassDesc>> PassDesc::GetVecFromData() const
{
	std::vector<const std::shared_ptr<PassDesc>> dataVec;
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		const std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data)
			dataVec.push_back(data);
	}
	return dataVec;
}

std::vector<std::shared_ptr<PassDesc>> PassDesc::GetVecFromOwners()
{
	std::vector<std::shared_ptr<PassDesc>> dataVec;
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data)
			dataVec.push_back(data);
	}
	return dataVec;
}

std::vector<const std::shared_ptr<PassDesc>> PassDesc::GetVecFromOwners() const
{
	std::vector<const std::shared_ptr<PassDesc>> dataVec;
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		const std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if (data)
			dataVec.push_back(data);
	}
	return dataVec;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PassDesc::InvalidateAttachments(std::shared_ptr<Pass> pass)
{
	//detach the children
	for (auto iter = m_Data.begin(); iter != m_Data.end(); iter++)
	{
		//all internal operations using Id
		//if this function is being called, the pass HAS to own this object
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> data = info->container.lock()->Find(info->id); //find it from its own pass
		if(!data)
		{
			//if there is no data, lets assume this passDesc has already been detached from
			//the parent and all the shit has been dealt with
			continue;
		}
			
		//remove this data from the owners list of its children data	
		for (auto dataIter = data->m_Owners.begin(); dataIter != data->m_Owners.end();)
		{
			if((*dataIter)->id == m_Id)
			{
				//remove
				data->m_Owners.erase(dataIter);
				break;
			}
			else
			{
				dataIter++;
			}
		}
	}
	m_Data.clear();
	
	//detach from parent
	for (auto iter = m_Owners.begin(); iter != m_Owners.end(); iter++)
	{
		PassDescInfo* info = (*iter);
		if (!info->container.lock())
		{
			continue;
		}
		std::shared_ptr<PassDesc> owner = info->container.lock()->Find(info->id); //find it from its own pass
		if(!owner)
		{
			continue;
		}
		
		//have the parent detach this from its data
		for (auto ownerIter = owner->m_Data.begin(); ownerIter != owner->m_Data.end();)
		{
			if ((*ownerIter)->id == m_Id)
			{
				//remove
				owner->m_Data.erase(ownerIter);
				break;
			}
			else
			{
				ownerIter++;
			}
		}
	}
	m_Owners.clear();
}