#pragma once
#include <memory>
#include <list>
#include <vector>
#include <utility>
#include <map>
#include <iostream>

#include "flFxConfig.h"
#include "GL.h"

class Pass;

typedef float fxId;
	
class PassDesc 
	: public std::enable_shared_from_this<PassDesc>
{
protected:
	bool m_Active;
	bool m_Validated;
	bool m_Modified; //if the PassDesc has been modified since validation
	bool m_IsInPass;

	const char* m_Name;
	fxState m_Flag;

	struct PassDescInfo
	{
		std::weak_ptr<Pass> container; //the pass that contains this data
		fxId id; //id of the data
	};

	std::weak_ptr<Pass> m_Parent;
	//std::list<std::weak_ptr<Pass>> m_Parents; //have multiple parents
	std::vector<PassDescInfo*> m_Owners; //use weak_ptrs instead of dependency
	//std::vector<fxId> m_Data; //use the id to handle evrything!!!!
	std::vector<PassDescInfo*> m_Data;
	
	unsigned int m_Mask;

	fxId m_Id;

	//std::weak_ptr<Effect> m_Effect; //new
	
	static fxId m_IdCounter; //each passDesc gets its unique ID
public:
	PassDesc(fxState flag,const char* name = NULL);
	virtual ~PassDesc();
	
	//PassDesc& operator=(PassDesc& other);

	virtual void SetName(const char* name) {m_Name = name;}
	virtual const char* GetName() const { return m_Name; }

	virtual void SetFlag(fxState flag) { m_Flag = flag; }
	virtual fxState GetFlag() const { return m_Flag; }
	
	virtual fxId GetId() const { return m_Id; }

	virtual void SetActive(bool active) { m_Active = active; }
	virtual bool IsActive() const { return m_Active; }

	virtual bool IsValid() const { return m_Validated; } //<-- put these as const funcs so const refs can access them

	virtual bool IsInPass() const { return m_IsInPass; } //<always check when attaching

	virtual bool HasChanged() const { return m_Modified; }

	virtual void SetParent(std::shared_ptr<Pass> parent) {m_Parent = parent;}
	virtual std::shared_ptr<Pass> GetParent() { return m_Parent.lock(); }

	void SetDataMask(unsigned int mask) { m_Mask = mask; }
	unsigned int GetDataMask() const { return m_Mask; }
	
	static fxId GenerateId(); //unique ID for this PassDesc
	
	std::shared_ptr<PassDesc> Copy() { return NULL; } //TODO
protected:
	virtual void SetValidated(bool validated) { m_Validated = validated; }
	
	virtual void NotifyChanged() { m_Modified = true; }
public:
	virtual bool Attach(std::shared_ptr<PassDesc> data);
	virtual bool Attach(const char* name); //remove flag!!!
	virtual bool Attach(fxId id); //add this

	virtual bool Attach(std::shared_ptr<Pass> container, const char* name); //new
	virtual bool Attach(std::shared_ptr<Pass> container, fxId id); //new*/

	virtual bool Detach(std::shared_ptr<PassDesc> data);
	virtual bool Detach(const char* name);
	virtual bool Detach(fxId id);

	//virtual bool Detach(std::shared_ptr<Pass> container, const char* name); //new
	//virtual bool Detach(std::shared_ptr<Pass> container, fxId id); //new

	//data : TODO: make finding by Id
	virtual std::shared_ptr<PassDesc> FindData(const char* name);
	virtual const std::shared_ptr<PassDesc> FindData(const char* name) const;

	virtual std::shared_ptr<PassDesc> FindData(std::shared_ptr<Pass> container, const char* name); //new
	virtual const std::shared_ptr<PassDesc> FindData(std::shared_ptr<Pass> container, const char* name) const;//new

	virtual std::shared_ptr<PassDesc> FindData(fxState flag);
	virtual const std::shared_ptr<PassDesc> FindData(fxState flag) const;

	virtual std::shared_ptr<PassDesc> FindData(std::shared_ptr<Pass> container, fxState flag);//new
	virtual const std::shared_ptr<PassDesc> FindData(std::shared_ptr<Pass> container, fxState flag) const;//new
	
	//id doesnt need a separate container overload cuz id is unique all across the board
	virtual std::shared_ptr<PassDesc> FindData(fxId id);
	virtual const std::shared_ptr<PassDesc> FindData(fxId id) const;

	virtual void FindData(fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec);
	virtual void FindData(fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const;

	virtual void FindData(std::shared_ptr<Pass> container, fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec);//new
	virtual void FindData(std::shared_ptr<Pass> container, fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const;//new

	virtual std::shared_ptr<PassDesc> FindData(int index); //this doesnt need overload since its index
	virtual const std::shared_ptr<PassDesc> FindData(int index) const;

	//now owners
	virtual std::shared_ptr<PassDesc> FindOwner(const char* name);
	virtual const std::shared_ptr<PassDesc> FindOwner(const char* name) const;

	virtual std::shared_ptr<PassDesc> FindOwner(std::shared_ptr<Pass> container, const char* name); //new
	virtual const std::shared_ptr<PassDesc> FindOwner(std::shared_ptr<Pass> container, const char* name) const; //new

	virtual std::shared_ptr<PassDesc> FindOwner(fxState flag);
	virtual const std::shared_ptr<PassDesc> FindOwner(fxState flag) const;

	virtual std::shared_ptr<PassDesc> FindOwner(std::shared_ptr<Pass> container, fxState flag); //new
	virtual const std::shared_ptr<PassDesc> FindOwner(std::shared_ptr<Pass> container, fxState flag) const; //new
	
	virtual std::shared_ptr<PassDesc> FindOwner(fxId id);
	virtual const std::shared_ptr<PassDesc> FindOwner(fxId id) const;

	virtual void FindOwner(fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec);
	virtual void FindOwner(fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const;

	virtual void FindOwner(std::shared_ptr<Pass> container, fxState flag, std::vector<std::shared_ptr<PassDesc>>& dataVec); //new
	virtual void FindOwner(std::shared_ptr<Pass> container, fxState flag, std::vector<const std::shared_ptr<PassDesc>>& dataVec) const; //new

	virtual std::shared_ptr<PassDesc> FindOwner(int index);
	virtual const std::shared_ptr<PassDesc> FindOwner(int index) const;

	//TODO: needs to be fixed
	virtual void ClearData() { m_Data.clear(); }
	
	virtual void ClearOwners() { m_Owners.clear(); }
	
	//void SaveAttachmentData();
	
	//void ClearAttachmentData();
	
	//bool RecalculateAttachments();

	std::vector<std::shared_ptr<PassDesc>> GetVecFromData();
	std::vector<const std::shared_ptr<PassDesc>> GetVecFromData() const;
	//make const versions
	std::vector<std::shared_ptr<PassDesc>> GetVecFromOwners();
	std::vector<const std::shared_ptr<PassDesc>> GetVecFromOwners() const;

	typedef std::vector<PassDescInfo*>::iterator PassDescIterator;
	typedef std::pair<PassDescIterator, PassDescIterator> PassDescIteratorPair;

	typedef std::vector<PassDescInfo*>::const_iterator ConstPassDescIterator;
	typedef std::pair<ConstPassDescIterator, ConstPassDescIterator> ConstPassDescIteratorPair;

	PassDescIteratorPair GetDataIterator() { return std::make_pair(m_Data.begin(), m_Data.end()); }
	ConstPassDescIteratorPair GetConstDataIterator() { return std::make_pair(m_Data.cbegin(), m_Data.cend()); }

	PassDescIteratorPair GetOwnerIterator() { return std::make_pair(m_Owners.begin(), m_Owners.end()); }
	ConstPassDescIteratorPair GetConstOwnerIterator() { return std::make_pair(m_Owners.cbegin(), m_Owners.cend()); }
	
	int GetDataSize() { return (int)m_Data.size(); }
	int GetOwnerSize() { return (int)m_Owners.size(); }
	
protected:
	virtual bool Validate() { SetValidated(true); return true; }

	virtual bool Execute() { return true; } 

	virtual bool Cleanup() { return true; } 

	virtual bool Invalidate() { SetValidated(false); return true; }

	virtual void ValidateAttachments(std::shared_ptr<Pass> pass) {} //called in Pass::Validate before the actual validation

	virtual void InvalidateAttachments(std::shared_ptr<Pass> pass); //implemented here
	
friend class Pass;
};