/******************************************************************************/
/*!
  \par        Project Octane
  \file       RenderSys.h
  \author     Zach Rammell
  \date       2020/10/22
  \brief      System responsible for 

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Graphics/GraphicsDeviceDX11.h>
#include <OctaneEngine/RenderComponent.h>
#include <OctaneEngine/Helper.h>

#include <EASTL/fixed_vector.h>
// Includes

namespace Octane
{

class RenderSys : public ISystem
{
public:
  explicit RenderSys(class Engine* parent_engine);

  virtual ~RenderSys() = default;

  void Load() override {}
  void LevelStart() override {}
  void Update() override;
  void LevelEnd() override {}
  void Unload() override {}

  // Intentionally unimplemented in the interface. Still necessary.
  static SystemOrder GetOrder();

  void SetClearColor(Color clear_color);

private:
  GraphicsDeviceDX11 device_dx11_;
  eastl::fixed_vector<MeshDX11, to_integral(MeshType::COUNT), false> meshes_;
};

}
