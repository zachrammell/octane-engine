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

#include <OctaneEngine/Graphics/GraphicsDeviceDX11.h>
#include <OctaneEngine/Helper.h>
#include <OctaneEngine/ISystem.h>

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

  static SystemOrder GetOrder();

  void OnResize();

  void SetClearColor(Color clear_color);
  GraphicsDeviceDX11 const& GetGraphicsDeviceDX11() const { return device_dx11_; }

private:
  GraphicsDeviceDX11 device_dx11_;
};

} // namespace Octane
