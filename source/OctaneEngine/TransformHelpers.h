//for making changes to transform components easy
#pragma once
#include <DirectXMath.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace dx = DirectX;

void SimpleMove(dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed);
void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, Octane::RigidBody& toPos, float speed);
void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed);
  //will go no lower than depth relative to target pos
void LockYRelToTarget(dx::XMFLOAT3& pos, const dx::XMFLOAT3& targetPos, float depth);
//chance between 0 and 10, 0 being 0%, 10 being 100%
void RandomJump(Octane::RigidBody& rb, const dx::XMFLOAT3& pos, float chance, float jumpForce);
