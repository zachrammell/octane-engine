#pragma once

#include <OctaneEngine/NBTWriter.h>
#include <OctaneEngine/NBTReader.h>

#include <OctaneEngine/Graphics/Colors.h>
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/Components/RenderComponent.h>
#include <OctaneEngine/Components/TransformComponent.h>
#include <OctaneEngine/Components/PhysicsComponent.h>

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
void NBTWriter::Write(string_view name, PhysicsComponent physics_component);

template<>
Color NBTReader::Read(string_view name);
template<>
DirectX::XMFLOAT3 NBTReader::Read(string_view name);
template<>
DirectX::XMFLOAT4 NBTReader::Read(string_view name);
//Todo: fix the multiple initialization error from this function calling NBTReader::ReadString()
//template<>
//Mesh_Key NBTReader::Read(string_view name);
template<>
ComponentKind NBTReader::Read(string_view name);

}
