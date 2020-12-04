#include <OctaneEngine/Components/PhysicsComponent.h>

namespace Octane
{
void PhysicsComponent::SetPosition(const DirectX::XMFLOAT3& position) const
{
  btTransform transform = rigid_body->getWorldTransform();
  transform.setOrigin(btVector3(position.x, position.y, position.z));
  rigid_body->setWorldTransform(transform);
}

void PhysicsComponent::SetRotation(const DirectX::XMFLOAT4& rotation) const
{
  btTransform transform = rigid_body->getWorldTransform();
  transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
  rigid_body->setWorldTransform(transform);
}

void PhysicsComponent::SetRotation(const DirectX::XMFLOAT3& euler_angle) const
{
  btTransform transform = rigid_body->getWorldTransform();
  transform.setRotation(btQuaternion(euler_angle.y, euler_angle.x, euler_angle.z));
  rigid_body->setWorldTransform(transform);
}

void PhysicsComponent::ApplyForce(const DirectX::XMFLOAT3& force) const
{
  if (rigid_body && rigid_body->getMotionState())
  {
    rigid_body->applyCentralForce(btVector3(force.x, force.y, force.z));
  }
}

void PhysicsComponent::ApplyTorque(const DirectX::XMFLOAT3& torque) const
{
  if (rigid_body && rigid_body->getMotionState())
  {
    rigid_body->applyTorque(btVector3(torque.x, torque.y, torque.z));
  }
}

void PhysicsComponent::SetGravity(float force) const 
{
  if (rigid_body && rigid_body->getMotionState())
  {
    rigid_body->setGravity(btVector3(0,force,0));
  }
}

} // namespace Octane
