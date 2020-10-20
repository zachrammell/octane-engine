
#include <OctaneEngine/TransformHelpers.h>

void SimpleMove(dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed)
{

  dx::XMVECTOR from {dx::XMLoadFloat3(&fromPos)};
  dx::XMVECTOR to {dx::XMLoadFloat3(&toPos)};

  const dx::XMVECTOR difference = dx::XMVectorSubtract(to, from);

  dx::XMVector3Dot(difference, difference);
  const float dist = dx::XMVector3Length(difference).m128_f32[0];

  if (!dx::XMScalarNearEqual(dist, 0.f, .000001f))
  {
    if (dist < speed)
      speed = dist;

  	dx::XMVECTOR move {dx::XMVector3Normalize(difference)};

  	move = dx::XMVectorScale(move, speed);

  	dx::XMStoreFloat3(&fromPos, dx::XMVectorAdd(from, move));
  }
}
