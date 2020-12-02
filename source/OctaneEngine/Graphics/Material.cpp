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
  Color color,
  Texture_Key diffuse_tex,
  Color spec,
  Texture_Key spec_tex,
  float spec_exp,
  bool tint_texture_with_color)
  : diffuse(color),
    diffuse_texture(diffuse_tex),
    specular(spec),
    specular_texture(spec_tex),
    specular_exp(spec_exp),
    tint(tint_texture_with_color)
  {
  }

Material::Material(const Material& rhs) 
: diffuse(rhs.diffuse),
  diffuse_texture(rhs.diffuse_texture),
  specular(rhs.specular),
  specular_texture(rhs.specular_texture),
  specular_exp(rhs.specular_exp),
  tint(rhs.tint)
{
    
}


} // namespace Octane
