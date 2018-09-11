#include "MotionState.h"

#include "Utils/Conversions/Vectors.h"

MotionState::MotionState (Transform* transform, const glm::vec3& offset) :
	_transform (transform),
	_offset (offset)
{

}

void MotionState::getWorldTransform (btTransform& worldTransform) const
{
	glm::vec3 position = _transform->GetPosition () + _offset;
	glm::quat rotation = _transform->GetRotation ();

	worldTransform.setOrigin (btVector3 (position.x, position.y, position.z));
	worldTransform.setRotation (btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
}

void MotionState::setWorldTransform (const btTransform& worldTransform)
{
	btVector3 position = worldTransform.getOrigin ();
	btQuaternion rotation = worldTransform.getRotation ();

	_transform->SetPosition (glm::vec3 (position.getX (), position.getY (), position.getZ ()));
	_transform->SetPosition (_transform->GetPosition () - _offset);

	_transform->SetRotation (glm::quat (rotation.getW (), rotation.getX (), rotation.getY (), rotation.getZ ()));
}