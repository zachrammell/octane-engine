/******************************************************************************/
/*!
  \par        Project Octane
  \file       Texture.h
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

#include <EASTL/string_view.h>
#include <EASTL/shared_ptr.h>

enum aiTextureType; //todo: delete this after we fix the build system

namespace Octane
{
typedef eastl::string_view Texture_Key; //TextureSys texture names


struct Texture
{
  winrt::com_ptr<ID3D11Texture2D> data = nullptr;
  winrt::com_ptr<ID3D11ShaderResourceView> view = nullptr;
  aiTextureType type;
  Texture_Key key;
  /*= aiTextureType::aiTextureType_NONE;*/ //todo: fix our build system so this can work
};

typedef Texture TextureDX11;
typedef eastl::shared_ptr<TextureDX11> TexturePtr;

const Texture_Key INVALID_TEXTURE {"Uninitialized"};
const Texture_Key UNTEXTURED {""};

} // namespace Octane
