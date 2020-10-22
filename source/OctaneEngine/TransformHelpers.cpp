
#include <algorithm>
#include <OctaneEngine/TransformHelpers.h>

void SimpleMove(dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed)
{

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
  dx::XMFLOAT3 to;
  toPos.SyncToPosition(to);

  SimpleMove(fromRB, fromPos, to, speed);
}

void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed)
{
  const dx::XMVECTOR difference = dx::XMVectorSubtract({toPos.x,toPos.y,toPos.z}, {fromPos.x,fromPos.y,fromPos.z});
  const float dist = dx::XMVector3Length(difference).m128_f32[0];

  if (!dx::XMScalarNearEqual(dist, 0.f, .000001f))
  {
    dx::XMVECTOR move {dx::XMVector3Normalize(difference)};

    move = dx::XMVectorScale(move, speed);
    move.m128_f32[1] = 0.f;
    fromRB.ApplyForceCentroid({move.m128_f32[0],move.m128_f32[1],move.m128_f32[2]});
  }
}

void LockYRelToTarget(dx::XMFLOAT3& pos, const dx::XMFLOAT3& targetPos, float depth)
{
  pos.y = std::clamp(pos.y, targetPos.y - depth, std::numeric_limits<float>::max());
}

void RandomJump(Octane::RigidBody& rb, const dx::XMFLOAT3& pos, float chance, float jumpForce)
{
  float jumpChance = rand() % 11;
  if (!dx::XMScalarNearEqual(pos.y,0.0f,0.250000f))
      return;
  if (chance >= jumpChance)
  {
    rb.ApplyForceCentroid({0.f, jumpForce, 0.f}); //SetLinearVelocity({0.f, jumpForce, 0.f}); //ApplyForce({0.f, jumpForce, 0.f}, {0.f, 1.f, 0.f});
  }
}
