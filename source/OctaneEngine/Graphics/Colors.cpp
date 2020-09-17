#include <OctaneEngine/Graphics/Colors.h>

namespace Octane
{

Color ColorFromRGB(float r, float g, float b)
{
  // simply scale interval from [0-255] to [0-1]
  return Color{ r / 255.0f, g / 255.0f, b / 255.0f };
}

Color ColorFromRGB(Color color_rgb)
{
  return ColorFromRGB(color_rgb.x, color_rgb.y, color_rgb.z);
}

}
