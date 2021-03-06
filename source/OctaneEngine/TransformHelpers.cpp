
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

void SimpleMove(Octane::PhysicsComponent& fromRB, dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed)
{
  //simplemove using physics
  const dx::XMVECTOR difference = dx::XMVectorSubtract({toPos.x, toPos.y, toPos.z}, {fromPos.x, fromPos.y, fromPos.z});
  const float dist = dx::XMVector3Length(difference).m128_f32[0];
  auto& yVel = fromRB.rigid_body->getLinearVelocity().y();
  if (!dx::XMScalarNearEqual(dist, 0.f, .000001f))
  {
    dx::XMVECTOR move {dx::XMVector3Normalize(difference)};

    move = dx::XMVectorScale(move, speed);
    //fromRB.ApplyForceCentroid();
    fromRB.rigid_body->setLinearVelocity({move.m128_f32[0], yVel, move.m128_f32[2]});
  }
}

void LockYRelToTarget(dx::XMFLOAT3& pos, const dx::XMFLOAT3& targetPos, float depth)
{
  pos.y = std::clamp(pos.y, targetPos.y - depth, std::numeric_limits<float>::max());
}

bool RandomJump(Octane::PhysicsComponent& rb, const dx::XMFLOAT3& pos, float chance, float jumpForce)
{
  float jumpChance = rand() % 101;
  if (!dx::XMScalarNearEqual(pos.y, 0.0f, 0.250000f))
    return false;
  if (chance >= jumpChance)
  {
    rb.ApplyForce({0.f, jumpForce, 0.f});
    return true;
  }
  return false;
}

void BunnyHop(Octane::PhysicsComponent& rb, const dx::XMFLOAT3& pos, float jumpForce)
{
  RandomJump(rb, pos, 100.f, jumpForce);
}

void FacePos(Octane::TransformComponent& obj, const dx::XMFLOAT3& pos)
{
  dx::XMVECTOR from {obj.pos.x, 0.f, obj.pos.z};
  dx::XMVECTOR to {pos.x, 0.f, pos.z};

  dx::XMVECTOR disp = dx::XMVectorSubtract(to, from);

  FaceDir(obj, {disp.m128_f32[0],disp.m128_f32[1],disp.m128_f32[2]});
}

void FaceDir(Octane::TransformComponent& obj, const dx::XMFLOAT3& dir)
{

  float pitch = /*0.f*/-dir.y;
  float yaw = atan2(dir.x, dir.z);
  float roll = 0.f;

  dx::XMStoreFloat4(&obj.rotation, dx::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
}

void PlaceRelativeTo(Octane::TransformComponent& obj,float offset,
  const dx::XMFLOAT3& relPos,const dx::XMVECTOR& invOrientation,const dx::XMVECTOR& direction)
{
  //placement
  dx::XMVECTOR offset_ {dx::XMVectorScale(direction,offset)};
  dx::XMStoreFloat3(&obj.pos, dx::XMVectorAdd(offset_, dx::XMLoadFloat3(&relPos)));
  //rotation
  ReorientTo(obj, invOrientation);
}

void ReorientTo(Octane::TransformComponent& obj, const dx::XMVECTOR& invOrientation)
{
  dx::XMStoreFloat4(&obj.rotation, invOrientation);
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
