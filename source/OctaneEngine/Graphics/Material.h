/******************************************************************************/
/*!
  \par        Project Octane
  \file       Material.h
  \author     Brayan Lopez
  \date       2020/11/24
  \brief      structure to contain material information

  Copyright ?2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <winrt/base.h>

#include <DirectXMath.h>
#include <d3d11.h>

#include <EASTL/vector.h>
#include <cstdint>

namespace dx = DirectX;
namespace Octane
{
constexpr int INVALID_SAMPLER = -1;
struct Material
{
  Material(
    dx::XMFLOAT3* color,
    int diffuse_texture = INVALID_SAMPLER,
    dx::XMFLOAT3 spec = {0.5f, 0.5f, 0.5f},
    int spec_texture = INVALID_SAMPLER,
    float spec_exp = 64.f,
    bool tint_texture_with_color = true);
  dx::XMFLOAT3* diffuse=nullptr;
  int diffuse_sampler;
  dx::XMFLOAT3 specular;
  int specular_sampler;
  float specular_exp;
  bool tint; //should texture be tinted by diffuse/specular?
};

} // namespace Octane