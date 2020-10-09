#pragma once
#include <DirectXMath.h>

namespace Octane
{

union Color
{
  struct
  {
    float r, g, b;
  };
  DirectX::XMFLOAT3 vec;
  bool operator!=(Color const& c) const
  {
    return !(r == c.r && g == c.g && b == c.b);
  }
};

// Makes a normalized color from an RGB triple in the range [0-255]
Color ColorFromRGB(uint8_t r, uint8_t g, uint8_t b);
// Makes a normalized color from an RGB color in the range [0-255]
Color ColorFromRGB(Color color_rgb);
// Creates a color from hexadecimal RGB. Highest 2 bytes are ignored. (0x00RRGGBB)
Color ColorFromHex(uint32_t rgb);

namespace Colors
{

// Harsh debugging colors

Color const black = {0.0f, 0.0f, 0.0f};
Color const white = {1.0f, 1.0f, 1.0f};
Color const red   = {1.0f, 0.0f, 0.0f};
Color const green = {0.0f, 1.0f, 0.0f};
Color const blue  = {0.0f, 0.0f, 1.0f};

// Pretty colors

Color const db32[32] = 
{
  ColorFromHex(0x000000), ColorFromHex(0x222034), ColorFromHex(0x45283c), ColorFromHex(0x663931),
  ColorFromHex(0x8f563b), ColorFromHex(0xdf7126), ColorFromHex(0xd9a066), ColorFromHex(0xeec39a),
  ColorFromHex(0xfbf236), ColorFromHex(0x99e550), ColorFromHex(0x6abe30), ColorFromHex(0x37946e),
  ColorFromHex(0x4b692f), ColorFromHex(0x524b24), ColorFromHex(0x323c39), ColorFromHex(0x3f3f74),
  ColorFromHex(0x306082), ColorFromHex(0x5b6ee1), ColorFromHex(0x639bff), ColorFromHex(0x5fcde4),
  ColorFromHex(0xcbdbfc), ColorFromHex(0xffffff), ColorFromHex(0x9badb7), ColorFromHex(0x847e87),
  ColorFromHex(0x696a6a), ColorFromHex(0x595652), ColorFromHex(0x76428a), ColorFromHex(0xac3232),
  ColorFromHex(0xd95763), ColorFromHex(0xd77bba), ColorFromHex(0x8f974a), ColorFromHex(0x8a6f30)
};

Color const peach    = ColorFromRGB(255, 163, 112);
Color const cerulean = ColorFromRGB(60, 166, 165);

// non-colors

Color const invalid = {-1, -1, -1};

} // namespace Colors

} // namespace Octane
