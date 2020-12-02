#include <OctaneEngine/Components/PhysicsComponent.h>

namespace Octane
{

void PhysicsComponent::SetRotation(DirectX::XMFLOAT4 q)
{
  btTransform bullet_transform;
  rigid_body->getMotionState()->getWorldTransform(bullet_transform);

  btQuaternion bt_rot(q.x, q.y, q.z, q.w);
  bullet_transform.setRotation(bt_rot);
  rigid_body->getMotionState()->setWorldTransform(bullet_transform);
}

} // namespace Octane
