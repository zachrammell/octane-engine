#include <OctaneEngine/Graphics/Colors.h>

namespace Octane
{

Color ColorFromRGB(uint8_t r, uint8_t g, uint8_t b)
{
  // simply scale interval from [0-255] to [0-1]
  return Color {r / 255.0f, g / 255.0f, b / 255.0f};
}

Color ColorFromRGB(Color color_rgb)
{
  return ColorFromRGB(color_rgb.x, color_rgb.y, color_rgb.z);
}

Color ColorFromHex(uint32_t rgb)
{
  return ColorFromRGB((rgb & 0x00FF0000) >> 16, (rgb & 0x0000FF00) >> 8, (rgb & 0x000000FF));
}

} // namespace Octane
