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

#include <OctaneEngine/RenderComponent.h>
#include <OctaneEngine/EntitySys.h>
#include <DirectXMath.h>

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
  bool entity_creator_ = true;
  bool entity_editor_ = true;
  struct EntityData
  {
    EntityID id = -1;
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 scale{1,1,1};
    DirectX::XMFLOAT3 rotation {};
    Color color{};
    MeshType mesh {MeshType::Cube};
  } entity_creator_data_, entity_editor_data_;
};

} // namespace Octane
