enum TransformType {
	LOCAL,
	GLOBAL
};
class NodeProperties
{
friend class Node;
protected:
	ActorId m_ActorId;
	glm::mat4 m_GlobalTransform;
	glm::mat4 m_GlobalAltTransform;
	glm::mat4 m_LocalTransform;
	glm::mat4 m_LocalAltTransform;
	float m_Radius;
	RenderPass m_RenderPass;
	Material m_Material;
	AlphaType m_AlphaType;

	void SetAlpha(const float alpha)
	{
		m_AlphaType = AlphaMaterial; m_Material.SetAlpha(alpha);
	}

public:
	NodeProperties(void);
	~NodeProperties(void) {}
	const ActorId &ActorId() const { return m_ActorId; }
	glm::mat4 const &GlobalTransform() const { return m_GlobalTransform; }
	glm::mat4 const &GlobalAltTransform() const { return m_GlobalAltTransform; }
	glm::mat4 const &LocalTransform() const { return m_LcoalTransform; }
	glm::mat4 const &LocallAltTransform() const { return m_LocalAltTransform; }
	void Transform(glm::mat4* toWorld, glm::mat4* fromWorld) const;

	bool HasAlpha() const { return m_Material.HasAlpha(); }
	float Alpha() const { return m_Material.GetAlpha(); }
	AlphaType AlphaType() const { return m_AlphaType; }

	RenderPass RenderPass() const { return m_RenderPass; }
	float Radius() const { return m_Radius; }

	Material GetMaterial() const { return m_Material; }
};
class Node : public std::enable_shared_from_this 
{
protected:
	enum TraversalType {
		NODE_INITIALIZE,
		NODE_UPDATE,
		NODE_CULLING,
		NODE_RENDER,
		NODE_DESTROY
	};
	
	bool m_NeedsInitialize; //set to true
	bool m_NeedsUpdate; //set to false
	bool m_NeedsCulling;
	bool m_NeedsRender; //set to true
	bool m_NeedsDestroy; //set to false
	
	bool m_InheritTransform;
	bool m_InheritScale;
	bool m_Active;
	bool m_Visible; //set to true default
	
	std::list<std::shared_ptr<Node>> m_Parents;
	
	std::list<std::shared_ptr<NodeVisitor>> m_QueuedVisitors; //remove from queue after visitor carried out duty
	
	std::map<TraversalType,std::shared_ptr<NodeCallback>> m_Callbacks;
	
	NodeProperties m_Props;
	
	const char* m_Name;
public:
	Node(ActorId actorId, RenderPass renderPass, glm::mat4* transform, glm::mat4* altTransform = NULL, const char* name = NULL);
	virtual ~Node();
	
	void SetName(const char* name);
	const char* GetName();
	
	virtual const NodeProperties* const GetProperties() const { return &m_Props; }
	
	virtual void Accept(std::shared_ptr<NodeVisitor> visitor);
	virtual void Cancel(std::shared_ptr<NodeVisitor> visitor); //only for queued visits
	virtual void Ascend(std::shared_ptr<NodeVisitor> visitor);
	virtual void Traverse(std::shared_ptr<NodeVisitor> visitor);
	
	virtual void AddParent(std::shared_ptr<Group> parent);
	virtual std::shared_ptr<Group> GetParent(int index);
	virtual const std::shared_ptr<Group> GetParent(int index) const;
	virtual std::list<std::shared_ptr<Group>> GetParents();
	virtual const std::list<std::shared_ptr<Group>> GetParents() const;
	virtual int GetNumParents();
	
	virtual void SetCallback(TraversalType type, std::shared_ptr<NodeCallback> callback);
	virtual void AddCallback(TraversalType type, std::shared_ptr<NodeCallback> callback);
	virtual void RemoveCallback(std::shared_ptr<NodeCallback> callback);
	virtual std::shared_ptr<NodeCallback> GetCallback(TraversalType type);
	virtual const std::shared_ptr<NodeCallback> GetCallback(TraversalType type) const;
	
	virtual void SetVisible(bool visible); //this is set by the culling callback
	virtual bool IsVisible();
	
	virtual bool SetNodeActive(bool active);
	virtual bool IsActive();
	
	virtual void FlagActive(TraversalType type);
	virtual void FlagInactive(TraversalType type);
	
	void SetRadius(int radius);
	const int GetRadius() const;
	
	//if has children, update the children too
	void SetTransform(TransformType relativeTo, glm::mat4* transform, glm::mat4* altTransform = NULL); 
	void MultiplyTransform(TransformType relativeTo, glm::mat4* transform, glm::mat4* altTransform = NULL);
	
	bool InheritTransform(bool inherit);
	const bool InheritsTranform();
	
	void SetMaterial(Material& mat);
	Material* GetMaterial();
	
	bool HasAlpha() const { return m_Material.HasAlpha(); }
	float Alpha() const { return m_Material.GetAlpha(); }
	AlphaType AlphaType() const { return m_AlphaType; }
	
	void SetPosition(glm::vec3& position, TransformType relativeTo);
	const glm::vec3 GetPosition(TransformType relativeTo) const;
	
	glm::vec3 ToWorldPosition(glm::vec3& localPosition);
	glm::vec3 ToLocalPosition(glm::vec3& worldPosition);
	
	virtual void Scale(glm::vec3& factor, TransformType relativeTo);
	virtual void Scale(float fx, float fy, float fz, TransformType relativeTo);
	virtual void Translate(glm::vec3& dest, TransformType relativeTo);
	virtual void Translate(float dx, float dy, float dz, TransformType relativeTo);
	virtual void Rotate(glm::vec3& axis, float angle, TransformType relativeTo);

	virtual void ResetTransform();
};
class NodeCallback : public std::enable_shared_from_this
{
friend class Node;
friend class Group;
protected:
	std::shared_ptr<NodeCallback> m_LinkedCallback;
public:
	NodeCallback();
	virtual ~NodeCallback();

	void SetLinkedCallback(std::shared_ptr<Node> callback);
	void AddLinkedCallback(std::shared_ptr<NodeCallback> callback);		
	std::shared_ptr<NodeCallback> GetLinkedCallback();
	const std::shared_ptr<NodeCallback> GetLinkedCallback() const;
	void RemoveLinkedCallback(std::shared_ptr<NodeCallback> callback);
	
	virtual bool operator()(std::shared_ptr<Node> node, std::shared_ptr<NodeVisitor> visitor);

	//the ones for Render
	virtual bool NodePreRendered(std::shared_ptr<Node> node); //set defaults for this
	virtual bool NodePostRendered(std::shared_ptr<Node> node);
};
class Group : public Node {
public:
	Group(/*...*/);
	virtual ~Group();
	
	typedef unsigned int Bitfield;
	bool Traverse(TraversalType type); //traverse and operate only on the nodes that requested
	bool Traverse(Bitfield type, bool separate); //for multiple traversal operations ex. if((type & NODE_UPDATE) == 0)...
	bool Ascend(TraversalType type, bool separate);
	bool Ascend(Bitfield type); 
	
	void AddChild(std::shared_ptr<Node> node);
	void InsertChild(std::shared_ptr<Node> node, int index);
	
	//gets and removes
	
	void CreateChild(/*pass constructor here*/);
};
class Switch : public Group {
protected:
	std::map<unsigned int, std::vector<int>> m_LevelMap; //index child positions to levels
public:
	Switch();
	~Switch();
	
	bool AddChild(std::shared_ptr<Node> node, int level);
	void InsertChild(std::shared_ptr<Node> node, int index, int level);
	bool RemoveChild(std::shared_ptr<Node> node, int level);
	
	std::shared_ptr<Node> GetChild(int index, int level);
	const std::shared_ptr<Node> GetChild(int index, int level) const;
	int GetChildLevel(std::shared_ptr<Node> node, int index);
	
	void EnableAllLevels();
	void DisableAllLevels();
	void EnableLevel(int level);
	void DisableLevel(int level);
	void EnableChild(int index, int level);
	void EnableSingleChild(int index, int level);
	void DisableChild(int index, int level);
};
class NodeVisitor : public std::enable_shared_from_this {
public:
	enum VisitingMode {
		VISIT_IMMEDIATLY,
		VISIT_ON_INITIALIZE,
		VISIT_ON_UPDATE,
		VISIT_ON_RENDER,
		VISIT_ON_DESTROY
	};
	
	void SetVisitingMode(VisitingMode vm);
	VisitingMode GetVisitingMode();
	
	virtual void Visit(std::shared_ptr<Node> node); 
	virtual void Visit(std::shared_ptr<Group> node);
	virtual void Visit(std::shared_ptr<Switch> node);
};
class NodeObserver : public std::enable_shared_from_this
{
public:
	virtual void Notify(std::shared_ptr<Node> node);
	virtual void Notify(std::shared_ptr<Group> node);
	virtual void Notify(std::shared_ptr<Switch> node);
};