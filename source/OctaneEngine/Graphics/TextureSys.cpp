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

#include <d3d11.h>

#include <OctaneEngine/Engine.h>
#include <OctaneEngine/Graphics/RenderSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/NBTReader.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/Graphics/stb_image.h>
#include <cstdio>
namespace dx = DirectX;

namespace Octane
{
TextureSys::TextureSys(class Engine* parent_engine) : ISystem(parent_engine)
{  
  //generate list of textures to load
  if(system(eastl::string{eastl::string{"dir "+path_}+"/B > " + datapath_.data()}.data()) == 0)
  {
    //std::ifstream file(datapath_.data());
    constexpr int max_name_length = 128;
    char line[max_name_length];
    //open that list we just made
    FILE* file = fopen(datapath_.data(),"r");
    if(file)
    {
      while(fgets(line,max_name_length,file))
      {
        eastl::string_view texture_path(line);
        auto end = texture_path.find(".");
        textureToPath_[{texture_path.begin(), texture_path.begin() + end}] = path_+"\\" + texture_path.data();
      }
    }
  }
  else
  {
    Trace::Log(Severity::FAILURE, "Failed to generate textures list\n");
  }

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

TextureDX11* TextureSys::Get(Texture_Key key)
{
  TexturePtr texture = textures_[key];

  if (texture)
  {
    return &*texture;
  }
  
  if(key == UNTEXTURED)
  {
    return nullptr;
  }

  auto loadedTex = LoadTexture(textureToPath_[eastl::string(key)]);

  return loadedTex;


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

TextureDX11* TextureSys::LoadTexture(eastl::string_view path)
{
  int width, height, channels;
  auto tex = stbi_load(path.data(), &width, &height, &channels, 4);

  if (!tex)
  {
    //return an invalid texture
    if(path != INVALID_TEXTURE)
    {
      Trace::Log(Severity::WARNING, "Couldn't find texture: %s to load, defaulting to \"uninitialized\" texture\n");
      return Get(INVALID_TEXTURE);
    }
    return nullptr;
  }
 
  auto& dx11device = reinterpret_cast<RenderSys*>(engine_.GetSystem(SystemOrder::RenderSys))->GetGraphicsDeviceDX11();
  auto device = dx11device.GetD3D11Device();
  auto deviceContext = dx11device.GetD3D11Context();
  TextureDX11* texture = new TextureDX11;

  if(!texture)
  {
    Trace::Log(Severity::ERROR, "Failed to create TextureDX11: %s", path);
  }

  D3D11_TEXTURE2D_DESC desc;
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 0;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

  HRESULT hr = device->CreateTexture2D(&desc, nullptr, texture->data.put());
  if (FAILED(hr))
  {
    Trace::Log(Severity::ERROR,"Failed to create texture: %s\n",path);
  }

  deviceContext->UpdateSubresource(texture->data.get(), 0, nullptr, tex, width * channels * sizeof(UCHAR), 0);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
  srvdesc.Format = desc.Format;
  srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvdesc.Texture2D.MostDetailedMip = 0;
  srvdesc.Texture2D.MipLevels = -1;

  // Create the shader resource view for the texture.
  hr = device->CreateShaderResourceView(texture->data.get(), &srvdesc, texture->view.put());
  if (FAILED(hr))
  {
    Trace::Log(Severity::ERROR,"Failed to create shader resource view for texture: %s\n",path);
  }

  // Generate mipmaps for this texture.
  deviceContext->GenerateMips(texture->view.get());

  textures_[path] = eastl::shared_ptr<TextureDX11>(texture);
  return texture;
}

} // namespace Octane
