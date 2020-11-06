#pragma once
#include <OctaneEngine/Components/PhysicsComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Physics/NarrowPhase.h>
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/PrimitivePair.h>
#include <OctaneEngine/Physics/ResolutionPhase.h>
#include <OctaneEngine/Physics/RigidBody.h>

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>

#include <btBulletCollisionCommon.h>


namespace Octane
{
class PhysicsSys final : public ISystem
{
  // ISystem implementation
public:
  explicit PhysicsSys(Engine* engine);
  ~PhysicsSys() = default;

  void Load() override {};

  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;

  void Unload() override {};

  static SystemOrder GetOrder();

public:
  void InitializeRigidBody(PhysicsComponent& compo);
  void AddPrimitive(PhysicsComponent& compo, ePrimitiveType type);
  void ErasePrimitive(PhysicsComponent& compo);
  eCollisionState HasCollision(PhysicsComponent& a, PhysicsComponent& b) const;
  bool HasCollision(
    const TransformComponent& transform_a,
    Primitive* primitive_a,
    const TransformComponent& transform_b,
    Primitive* primitive_b,
    size_t exit_count = 100);

private:
  eastl::vector<Primitive*> primitives_;
  eastl::vector<PrimitivePair> potential_pairs_;
  eastl::hash_map<size_t, ContactManifold> manifold_table_;
  eastl::hash_map<size_t, eCollisionState> collision_table_;

  NarrowPhase narrow_phase_;
  ResolutionPhase resolution_phase_;

  void* bt_broad_phase_ = nullptr;
  void* bt_collision_config_ = nullptr;
  void* bt_narrow_phase_ = nullptr;
  void* bt_resolution_phase_ = nullptr;
  void* bt_world_ = nullptr;

};
} // namespace Octane
