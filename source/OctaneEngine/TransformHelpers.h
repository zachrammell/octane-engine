//for making changes to transform components easy
#pragma once
#include <DirectXMath.h>
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/TransformComponent.h>

namespace dx = DirectX;

void SimpleMove(dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed);
void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, Octane::RigidBody& toPos, float speed);
void SimpleMove(Octane::RigidBody& fromRB, dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed);
  //will go no lower than depth relative to target pos
void LockYRelToTarget(dx::XMFLOAT3& pos, const dx::XMFLOAT3& targetPos, float depth);
//chance between 0 and 10, 0 being 0%, 10 being 100%
void RandomJump(Octane::RigidBody& rb, const dx::XMFLOAT3& pos, float chance, float jumpForce);
void BunnyHop(Octane::RigidBody& rb, const dx::XMFLOAT3& pos,float jumpForce);
void FacePos(Octane::TransformComponent& obj, const dx::XMFLOAT3& pos);

dx::XMVECTOR LocalToWorldPoint(const Octane::TransformComponent& transform, const dx::XMVECTOR& local_point);
dx::XMVECTOR WorldToLocalPoint(const Octane::TransformComponent& transform, const dx::XMVECTOR& world_point);
dx::XMVECTOR LocalToWorldVector(const Octane::TransformComponent& transform, const dx::XMVECTOR& local_vector);
dx::XMVECTOR WorldToLocalVector(const Octane::TransformComponent& transform, const dx::XMVECTOR& world_vector);
