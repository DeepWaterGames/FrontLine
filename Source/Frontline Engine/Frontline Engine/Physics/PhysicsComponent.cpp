#include "PhysicsComponent.h"

const std::string PhysicsComponent::g_name = "PhysicsComponent";

PhysicsComponent::PhysicsComponent()
{
}


PhysicsComponent::~PhysicsComponent()
{
}

// --------------------------------------------------------------------------
// Function:		PhysicsComponent::VGenerateXML
// Purpose:			Convert the component to xml
// Parameters:		None
// --------------------------------------------------------------------------
pugi::xml_node PhysicsComponent::VGenerateXML()
{
	pugi::xml_node ln_outNode;

	ln_outNode.set_name(VGetComponentName().c_str());

	std::stringbuf l_retValBuffer;
	std::ostream l_xmlStream(&l_retValBuffer);

	return ln_outNode;
}

// --------------------------------------------------------------------------
// Function:		PhysicsComponent::VInit
// Purpose:			Initialize the component with an xml node
// Parameters:		The xml node to be used
// --------------------------------------------------------------------------
bool PhysicsComponent::VInit(pugi::xml_node pn_data)
{
	std::shared_ptr<IPhysics> l_strongPhysicsPtr = IGame::gp_game->mp_physics;
	mp_gamePhysics = std::weak_ptr<IPhysics>(l_strongPhysicsPtr);
	if (!l_strongPhysicsPtr) return false;

	pugi::xml_node ln_shape = pn_data.child("Shape").first_child();
	if (ln_shape.value() != "")
	{
		ms_shape = ln_shape.value();
		mn_shapeInfo = ln_shape;
	}
	pugi::xml_node ln_density = pn_data.child("Density").first_child();
	if (ln_density.value() != "")
	{
		ms_density = ln_density.value();
	}
	pugi::xml_node ln_material = pn_data.child("Material").first_child();
	if (ln_material.value() != "")
	{
		ms_material = ln_material.value();
	}
	pugi::xml_node ln_transform = pn_data.child("Transform");
	
	BuildRigidBodyTransform(ln_transform);


	pugi::xml_node ln_channels = pn_data.child("CollisionChannels");

	m_collisionType = atoi(ln_channels.child("Type").first_child().value());
	m_collidesWith = atoi(ln_channels.child("CollideWith").first_child().value());

	return true;
}

// --------------------------------------------------------------------------
// Function:		PhysicsComponent::BuildRigidBodyTransform
// Purpose:			A helper function to build a transform matrix using an xml node
// Parameters:		The xml node to be used
// --------------------------------------------------------------------------
void PhysicsComponent::BuildRigidBodyTransform(pugi::xml_node pn_data)
{
	glm::vec3 lv_translate, lv_rotate, lv_scale;
	pugi::xml_node ln_translation = pn_data.child("Translation");
	//if (ln_translation.value() != "")
	{
		if (ln_translation.child("x").first_child().value() != "")
			lv_translate.x = atof(ln_translation.child("x").first_child().value());
		if (ln_translation.child("y").first_child().value() != "")
			lv_translate.x = atof(ln_translation.child("y").first_child().value());
		if (ln_translation.child("z").first_child().value() != "")
			lv_translate.x = atof(ln_translation.child("z").first_child().value());
	}
	pugi::xml_node ln_rotation = pn_data.child("Rotation");
	//if (ln_rotation.value() != "")
	{
		if (ln_rotation.child("x").first_child().value() != "")
			lv_rotate.x = atof(ln_rotation.child("x").first_child().value());
		if (ln_rotation.child("y").first_child().value() != "")
			lv_rotate.y = atof(ln_rotation.child("y").first_child().value());
		if (ln_rotation.child("z").first_child().value() != "")
			lv_rotate.z = atof(ln_rotation.child("z").first_child().value());
	}
	pugi::xml_node ln_scale = pn_data.child("Scale");
	//if (ln_scale.value() != "")
	{
		if (ln_scale.child("x").first_child().value() != "")
			lv_scale.x = atof(ln_scale.child("x").first_child().value());
		if (ln_scale.child("y").first_child().value() != "")
			lv_scale.y = atof(ln_scale.child("y").first_child().value());
		if (ln_scale.child("z").first_child().value() != "")
			lv_scale.z = atof(ln_scale.child("z").first_child().value());
	}

	glm::mat4 lm_translate = glm::translate(lv_translate);
	glm::mat4 lm_rotate = glm::translate(lv_rotate);
	glm::mat4 lm_scale = glm::translate(lv_scale);

	mm_transform = lm_scale * lm_rotate;
	mm_transform *= lm_translate;
}

// --------------------------------------------------------------------------
// Function:		PhysicsComponent::VPostInit
// Purpose:			Finish component initialization
// Parameters:		None
// --------------------------------------------------------------------------
void PhysicsComponent::VPostInit()
{
	if (m_owner)
	{
		if (ms_shape == "Sphere")
		{
			MakeStrongPtr<IPhysics>(mp_gamePhysics)->VAddSphere(getScaleFromMat(mm_transform).x, std::weak_ptr<Actor>(m_owner), ms_density, ms_material, m_collisionType, m_collidesWith);
		}
		else if (ms_shape == "Box")
		{
			MakeStrongPtr<IPhysics>(mp_gamePhysics)->VAddBox(getScaleFromMat(mm_transform), std::weak_ptr<Actor>(m_owner), ms_density, ms_material, m_collisionType, m_collidesWith);
		}
		else if (ms_shape == "Cylinder")
		{
			MakeStrongPtr<IPhysics>(mp_gamePhysics)->VAddCylinder(getScaleFromMat(mm_transform).x, getScaleFromMat(mm_transform).y, std::weak_ptr<Actor>(m_owner), ms_density, ms_material, m_collisionType, m_collidesWith);
		}
		else if (ms_shape == "PointCloud")
		{
			ERRLOG("Physics", "Point Cloud not supported");
		}
	}
}