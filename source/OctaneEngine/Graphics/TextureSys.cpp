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

#include <OctaneEngine/Graphics/TextureSys.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/Trace.h>

namespace dx = DirectX;

namespace Octane
{
TextureSys::TextureSys(class Engine* parent_engine) : ISystem(parent_engine)
{  
  //auto& device = reinterpret_cast<RenderSys*>(engine_.GetSystem(SystemOrder::RenderSys))->GetGraphicsDeviceDX11();
  //NBTReader read(datapath_);
  //
  //path_ = read.ReadString("PathPrefix");
  //if(path_.empty())
  //{
  //  Trace::Log(Severity::ERROR, "TextureSys::TextureSys, failed to read path to textures from textures NBT\n");
  //}

  //if(read.OpenList("Data"))
  //{
  //  const int textureListSize = read.ListSize();
  //  texturenames_.resize(textureListSize);
  //  for(int i = 0; i < textureListSize; ++i)
  //  {
  //    if(read.OpenCompound(""))
  //    {
  //      const eastl::string name{read.ReadString("name")};
  //      texturenames_[i] = name;
  //      textureToPath_[name] = path_ + eastl::string(read.ReadString("path"));
  //      read.CloseCompound();
  //    }
  //  }
  //  read.CloseList();
  //}
}

TextureSys::~TextureSys() {}

SystemOrder TextureSys::GetOrder()
{
  return SystemOrder::TextureSys;
}

const eastl::vector<eastl::string>& TextureSys::TextureNames() const
{
  return texturenames_;
}

const TextureDX11 TextureSys::Get(Texture_Key key)
{
  TextureDX11 texture = textures_[key];

  if (texture != INVALID_SAMPLER)
  {
    return texture;
  }
  
  //auto& device = reinterpret_cast<RenderSys*>(engine_.GetSystem(SystemOrder::RenderSys))->GetGraphicsDeviceDX11();

  //device.EmplaceTexture(textures_, key, LoadTexture(textureToPath_[eastl::string{key}].data()));
  //texture = textures_.find(key)->second;

  //if(texture)
  //{
  //  return &*texture;  
  //}

  //Trace::Log(Severity::WARNING, "TextureSys::Get, Tried to load Texture: %s which doesn't exist in %s", key, datapath_);

  //return nullptr;
}

const eastl::vector<eastl::string>& TextureSys::TextureNames() const
{
  return texturenames_;
}

} // namespace Octane
