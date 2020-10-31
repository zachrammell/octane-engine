#pragma once
#include <OctaneEngine/Physics/Primitive.h>
#include <OctaneEngine/Physics/RigidBody.h>

namespace Octane
{
struct PhysicsComponent
{
  RigidBody rigid_body;
  Primitive* primitive = nullptr;
  PhysicsSys* sys = nullptr;
};
} // namespace Octane
