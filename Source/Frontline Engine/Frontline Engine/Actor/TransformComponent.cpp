#include "TransformComponent.h"

const std::string TransformComponent::g_name = "TransformComponent";

TransformComponent::TransformComponent()
{
}


TransformComponent::~TransformComponent()
{
}

glm::vec3 TransformComponent::FindPosition()
{
	glm::vec3 l_retval;
	l_retval = glm::vec3(mm_transform[0][3], mm_transform[1][3], mm_transform[2][3]);
	return l_retval;
}


glm::vec3 TransformComponent::FindRotation()
{
	glm::vec3 l_retval;
	l_retval = glm::vec3(mm_transform[0][3], mm_transform[1][3], mm_transform[2][3]);
	return l_retval;
}


glm::vec3 TransformComponent::FindScale()
{
	glm::vec3 l_retval;
	l_retval = glm::vec3(mm_transform[0][3], mm_transform[1][3], mm_transform[2][3]);
	return l_retval;
}

bool TransformComponent::VInit(pugi::xml_node pn_data)
{
	glm::vec3 lv_translate, lv_rotate, lv_scale;

	pugi::xml_node ln_translation = pn_data.child("Translation");
	if (ln_translation.child("x").first_child().value() != "")
		lv_translate.x = atof(ln_translation.child("x").first_child().value());
	if (ln_translation.child("y").first_child().value() != "")
		lv_translate.y = atof(ln_translation.child("y").first_child().value());
	if (ln_translation.child("z").first_child().value() != "")
		lv_translate.z = atof(ln_translation.child("z").first_child().value());

	pugi::xml_node ln_rotation = pn_data.child("Rotation");
	if (ln_rotation.child("x").first_child().value() != "")
		lv_rotate.x = atof(ln_rotation.child("x").first_child().value());
	if (ln_rotation.child("y").first_child().value() != "")
		lv_rotate.y = atof(ln_rotation.child("y").first_child().value());
	if (ln_rotation.child("z").first_child().value() != "")
		lv_rotate.z = atof(ln_rotation.child("z").first_child().value());

	pugi::xml_node ln_scale = pn_data.child("Scale");
	if (ln_scale.child("x").first_child().value() != "")
		lv_scale.x = atof(ln_scale.child("x").first_child().value());
	if (ln_scale.child("y").first_child().value() != "")
		lv_scale.y = atof(ln_scale.child("y").first_child().value());
	if (ln_scale.child("z").first_child().value() != "")
		lv_scale.z = atof(ln_scale.child("z").first_child().value());

	glm::mat4 lm_translate = glm::translate(lv_translate);
	glm::mat4 lm_rotate = glm::translate(lv_rotate);
	glm::mat4 lm_scale = glm::translate(lv_scale);

	mm_transform = glm::mat4(1) * lm_rotate * lm_scale;
	mm_transform *= lm_translate;

	return true;
}
void TransformComponent::VPostInit()
{
}