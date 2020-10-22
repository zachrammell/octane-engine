#pragma once

#include <OctaneEngine/NBTWriter.h>

#include <OctaneEngine/Graphics/Colors.h>
#include <DirectXMath.h>

namespace Octane
{

template<>
void NBTWriter::Write(string_view name, Color color);
template<>
void NBTWriter::Write(string_view name, DirectX::XMFLOAT3 vec);
template<>
void NBTWriter::Write(string_view name, DirectX::XMFLOAT4 vec);

}
