//for making changes to transform components easy
#pragma once
#include <DirectXMath.h>

namespace dx = DirectX;

void SimpleMove(dx::XMFLOAT3& fromPos, dx::XMFLOAT3& toPos, float speed);
