#pragma once
#include <DirectXMath.h>

namespace Octane
{

using Color = DirectX::XMFLOAT3;

// Makes a normalized color from an RGB triple in the range [0-255]
Color ColorFromRGB(float r, float g, float b);
// Makes a normalized color from an RGB color in the range [0-255]
Color ColorFromRGB(Color color_rgb);

namespace Colors
{

// Harsh debugging colors

Color const black = {0.0f, 0.0f, 0.0f};
Color const white = {1.0f, 1.0f, 1.0f};
Color const red   = {1.0f, 0.0f, 0.0f};
Color const green = {0.0f, 1.0f, 0.0f};
Color const blue  = {0.0f, 0.0f, 1.0f};

// Pretty colors

Color const peach    = ColorFromRGB(255, 163, 112);
Color const cerulean = ColorFromRGB(60, 166, 165);

} // namespace Colors

} // namespace Octane
