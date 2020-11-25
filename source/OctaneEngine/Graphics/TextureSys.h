/******************************************************************************/
/*!
  \par        Project Octane
  \file       TextureSys.h
  \author     Brayan Lopez
  \date       2020/11/24
  \brief      Classes to contain information for textures

  Copyright ?2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <Windows.h>
#include <d3d11.h>
#include <winrt/base.h>

#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Helper.h>
#include <OctaneEngine/Components/RenderComponent.h>

#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
struct aiScene;
struct aiNode;
struct aiMesh;


namespace Octane
{

  struct Texture
  {
    winrt::com_ptr<ID3D11Texture2D> data;
    winrt::com_ptr<ID3D11ShaderResourceView> view;
  };

  typedef int TextureDX11;

class TextureSys : public ISystem
{
public:

  explicit TextureSys(class Engine* parent_engine);
  ~TextureSys();
  //MeshSys(ID3D11Device* device, ID3D11DeviceContext* context);

  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update() {};
  virtual void LevelEnd() {};
  virtual void Unload() {};

  static SystemOrder GetOrder();
 
  const eastl::vector<eastl::string>& TextureNames() const;
  TextureDX11 Get(Texture_Key key);

private:
  eastl::hash_map<Texture_Key, TextureDX11> textures_;
  eastl::hash_map<eastl::string, eastl::string> textureToPath_; //maps mesh names to paths
  eastl::string_view datapath_{"assets/textures.nbt"}; //path to textures NBT
  eastl::string path_; //path to textures
  eastl::vector<eastl::string> texturenames_;
  TextureDX11 LoadTexture(const char* path);
};

} // namespace Octane
