
#include <OctaneEngine/Math.h>
#include <OctaneEngine/TransformHelpers.h>
#include <algorithm>

void SimpleMove(dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed)
{
  //simplemove using transform, no physics
  dx::XMVECTOR from {dx::XMLoadFloat3(&fromPos)};
  dx::XMVECTOR to {dx::XMLoadFloat3(&toPos)};

  const dx::XMVECTOR difference = dx::XMVectorSubtract(to, from);

  //dx::XMVector3Dot(difference, difference);
  const float dist = dx::XMVector3Length(difference).m128_f32[0];

  if (!dx::XMScalarNearEqual(dist, 0.f, .000001f))
  {
    //if (dist < speed)
    //  speed = dist;

    dx::XMVECTOR move {dx::XMVector3Normalize(difference)};

    move = dx::XMVectorScale(move, speed);
    move.m128_f32[1] = 0.f;
    dx::XMStoreFloat3(&fromPos, dx::XMVectorAdd(from, move));
  }
}

void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, Octane::RigidBody& toPos, float speed)
{
  //simplemove using physics
  dx::XMFLOAT3 to = toPos.GetPosition();

  SimpleMove(fromRB, fromPos, to, speed);
}

void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed)
{
  //simplemove using physics
  const dx::XMVECTOR difference = dx::XMVectorSubtract({toPos.x, toPos.y, toPos.z}, {fromPos.x, fromPos.y, fromPos.z});
  const float dist = dx::XMVector3Length(difference).m128_f32[0];
  auto& yVel = fromRB.GetLinearVelocity().m128_f32[1];
  if (!dx::XMScalarNearEqual(dist, 0.f, .000001f))
  {
    dx::XMVECTOR move {dx::XMVector3Normalize(difference)};

    move = dx::XMVectorScale(move, speed);
    //fromRB.ApplyForceCentroid();
    fromRB.SetLinearVelocity({move.m128_f32[0], yVel, move.m128_f32[2]});
  }
}

void LockYRelToTarget(dx::XMFLOAT3& pos, const dx::XMFLOAT3& targetPos, float depth)
{
  pos.y = std::clamp(pos.y, targetPos.y - depth, std::numeric_limits<float>::max());
}

void RandomJump(Octane::RigidBody& rb, const dx::XMFLOAT3& pos, float chance, float jumpForce)
{
  float jumpChance = rand() % 11;
  if (!dx::XMScalarNearEqual(pos.y, 0.0f, 0.250000f))
    return;
  if (chance >= jumpChance)
  {
    rb.ApplyForceCentroid(
      {0.f,
       jumpForce,
       0.f}); //SetLinearVelocity({0.f, jumpForce, 0.f}); //ApplyForce({0.f, jumpForce, 0.f}, {0.f, 1.f, 0.f});
  }
}

void BunnyHop(Octane::RigidBody& rb, const dx::XMFLOAT3& pos, float jumpForce)
{
  RandomJump(rb, pos, 100.f, jumpForce);
}

void FacePos(Octane::TransformComponent& obj, const dx::XMFLOAT3& pos)
{
  dx::XMVECTOR from {obj.pos.x, 0.f, obj.pos.z};
  dx::XMVECTOR to {pos.x, 0.f, pos.z};
  //dx::XMMATRIX lookatMat = dx::XMMatrixLookAtLH(from, to, {0.f, 1.f, 0.f});
  //dx::XMMATRIX invMat = dx::XMMatrixInverse(nullptr,lookatMat);
  //dx::XMVECTOR rot = dx::XMQuaternionRotationMatrix(invMat);
  //dx::XMStoreFloat4(&obj.rotation,rot);

  dx::XMVECTOR disp = dx::XMVectorSubtract(to, from);

  float pitch = 0.f; /*asin(-disp.m128_f32[1]);*/
  float yaw = atan2(disp.m128_f32[0], disp.m128_f32[2]);
  float roll = 0.f;

  dx::XMStoreFloat4(&obj.rotation, dx::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
}

dx::XMVECTOR LocalToWorldPoint(const Octane::TransformComponent& transform, const dx::XMVECTOR& local_point)
{
  dx::XMVECTOR orientation = XMLoadFloat4(&transform.rotation);
  dx::XMVECTOR position = XMLoadFloat3(&transform.pos);
  return DirectX::XMVectorAdd(Octane::Math::Rotate(orientation, local_point), position);
}

dx::XMVECTOR WorldToLocalPoint(const Octane::TransformComponent& transform, const dx::XMVECTOR& world_point)
{
  dx::XMVECTOR orientation = XMLoadFloat4(&transform.rotation);
  dx::XMVECTOR position = XMLoadFloat3(&transform.pos);
  return Octane::Math::Rotate(dx::XMQuaternionInverse(orientation), DirectX::XMVectorSubtract(world_point, position));
}

dx::XMVECTOR LocalToWorldVector(const Octane::TransformComponent& transform, const dx::XMVECTOR& local_vector)
{
  dx::XMVECTOR orientation = XMLoadFloat4(&transform.rotation);
  return Octane::Math::Rotate(orientation, local_vector);
}

dx::XMVECTOR WorldToLocalVector(const Octane::TransformComponent& transform, const dx::XMVECTOR& world_vector)
{
  dx::XMVECTOR orientation = XMLoadFloat4(&transform.rotation);
  return Octane::Math::Rotate(dx::XMQuaternionInverse(orientation), world_vector);
}
