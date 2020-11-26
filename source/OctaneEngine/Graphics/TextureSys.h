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

#include <OctaneEngine/Graphics/Texture.h>
#include <OctaneEngine/ISystem.h>
#include <OctaneEngine/Helper.h>

#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>

struct aiScene;
struct aiNode;
struct aiMesh;

namespace Octane
{

class TextureSys : public ISystem
{
public:

  explicit TextureSys(class Engine* parent_engine);
  ~TextureSys();

  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update() {};
  virtual void LevelEnd() {};
  virtual void Unload() {};

  static SystemOrder GetOrder();
 
  const eastl::vector<eastl::string>& TextureNames() const;
  TextureDX11* Get(Texture_Key key);
  Texture_Key AddTexture(eastl::string_view path, aiTextureType type = aiTextureType::aiTextureType_DIFFUSE);

private:
  eastl::hash_map<Texture_Key, TexturePtr> textures_;
  eastl::hash_map<eastl::string, eastl::string> textureToPath_; //maps texture names to paths
  eastl::string_view datapath_{"assets\\textures.txt"}; //path to textures txt
  eastl::string path_ {"assets\\Textures"}; //path to textures
  eastl::vector<eastl::string> texturenames_;
  TextureDX11* LoadTexture(eastl::string_view path);
};

} // namespace Octane
