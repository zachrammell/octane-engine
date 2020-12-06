/******************************************************************************/
/******************************************************************************/
/*!
  \par        Project Octane
  \file       SerializationTestScene.h
  \author     Zach Rammell
  \date       2020/10/21
  \brief      test scene for serializing stuff

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes

#include <OctaneEngine/SceneSys.h>

#include <DirectXMath.h>
#include <EASTL/string.h>
#include <OctaneEngine/Components/RenderComponent.h>
#include <OctaneEngine/EntityID.h>
namespace Octane
{

class SerializationTestScene final : public IScene
{
public:
  explicit SerializationTestScene(SceneSys* parent);

  virtual void Load();
  virtual void Start();
  virtual void End();
  virtual void Unload();

  virtual void Update(float dt);
  virtual SceneE GetEnum() const;

private:
  float slider_sensitivity = 0.1f;
  bool entity_creator_ = true;
  bool entity_editor_ = true;
  bool editor_settings_ = true;
  bool camera_moving_ = false;
  struct EntityData
  {
    EntityID id = INVALID_ENTITY;
    DirectX::XMFLOAT3 position {};
    DirectX::XMFLOAT3 scale {1, 1, 1};
    DirectX::XMFLOAT3 rotation {};
    Color color {};
    Mesh_Key mesh = Mesh_Key {"Cube"};
    bool has_collider = false;
    eastl::string name {32, '\0'};
    RenderComponent* render_comp = nullptr;
  } entity_creator_data_, entity_editor_data_;
};

} // namespace Octane
