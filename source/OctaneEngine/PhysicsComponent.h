#pragma once
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{
struct PhysicsComponent
{
  Octane::RigidBody rigid_body;
  Octane::Primitive* primitive = nullptr;
};
} // namespace Octane
