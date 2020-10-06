#include <OctaneEngine/Physics/Primitive.h>

namespace Octane
{
Primitive::Primitive() : local_centroid_(), local_inertia_() {}

Primitive::~Primitive() {}

float Primitive::GetDensity() const
{
  return density_;
}

void Primitive::SetMass(float density)
{
  density_ = density;
}

RigidBody* Primitive::GetRigidBody() const
{
  return rigid_body_;
}
} // namespace Octane
