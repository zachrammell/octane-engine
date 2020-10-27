#pragma once
#include <OctaneEngine/Physics/RigidBody.h>
#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{
struct PhysicsComponent
{
  RigidBody rigid_body;
  Primitive* primitive = nullptr;
};
} // namespace Octane
