#pragma once

#include <OctaneEngine/NBTWriter.h>
#include <OctaneEngine/NBTReader.h>

#include <OctaneEngine/Graphics/Colors.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/RenderComponent.h>
#include <OctaneEngine/TransformComponent.h>

#include <DirectXMath.h>

namespace Octane
{

template<>
void NBTWriter::Write(string_view name, Color color);
template<>
void NBTWriter::Write(string_view name, DirectX::XMFLOAT3 vec);
template<>
void NBTWriter::Write(string_view name, DirectX::XMFLOAT4 vec);
template<>
void NBTWriter::Write(string_view name, RenderComponent render_component);
template<>
void NBTWriter::Write(string_view name, TransformComponent transform_component);

template<>
Color NBTReader::Read(string_view name);
template<>
DirectX::XMFLOAT3 NBTReader::Read(string_view name);
template<>
DirectX::XMFLOAT4 NBTReader::Read(string_view name);
template<>
MeshType NBTReader::Read(string_view name);
template<>
ComponentKind NBTReader::Read(string_view name);

}
