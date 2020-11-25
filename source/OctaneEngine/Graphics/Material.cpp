/******************************************************************************/
/*!
  \par        Project Octane
  \file       Material.cpp
  \author     Brayan Lopez
  \date       2020/11/24
  \brief      structure to contain material information

  Copyright ?2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/Graphics/Material.h>

namespace Octane
{

Material::Material(
  dx::XMFLOAT3* color,
  int diffuse_texture = INVALID_SAMPLER,
  dx::XMFLOAT3 spec = {0.5f, 0.5f, 0.5f},
  int spec_texture = INVALID_SAMPLER,
  float spec_exp = 64.f,
  bool tint_texture_with_color = true)
  : diffuse(color),
    diffuse_sampler(diffuse_texture),
    specular(spec),
    specular_sampler(spec_texture),
    specular_exp(spec_exp),
    tint(tint_texture_with_color)
  {
  }

} // namespace Octane
