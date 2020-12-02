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
#include <OctaneEngine/Graphics/Texture.h>
#include <OctaneEngine/Graphics/Colors.h>
#include <EASTL/vector.h>
#include <cstdint>

namespace dx = DirectX;
namespace Octane
{
struct Material
{
  Material(
    Color color = Color{0.5f,0.88f,0.85f},
    Texture_Key diffuse_tex = UNTEXTURED,
    Color spec = {0.5f, 0.5f, 0.5f},
    Texture_Key spec_tex = UNTEXTURED,
    float spec_exp = 64.f,
    bool tint_texture_with_color = true);
  Color diffuse{};
  Texture_Key diffuse_texture;
  Color specular;
  Texture_Key specular_texture;
  float specular_exp;
  bool tint; //should texture be tinted by diffuse/specular?
};

} // namespace Octane
