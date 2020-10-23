#pragma once

#include <OctaneEngine/ISystem.h>

#include <EASTL/unique_ptr.h>
#include <SDL.h>

#include <DirectXMath.h>

namespace Octane
{

class InputHandler final : public ISystem
{
  // ISystem implementation
public:
  explicit InputHandler(class Engine* engine);
  ~InputHandler() = default;

  void Load() override {};
  void LevelStart() override {};
  void Update() override;
  void LevelEnd() override {};
  void Unload() override {};

  static SystemOrder GetOrder();

  enum KeyState
  {
    IDLE,
    PRESSED,
    HELD,
    RELEASED,
  };

  bool KeyPressed(SDL_KeyCode key);
  bool KeyHeld(SDL_KeyCode key);
  bool KeyPressedOrHeld(SDL_KeyCode key);
  bool KeyReleased(SDL_KeyCode key);

  DirectX::XMINT2 GetMouseMovement();

private:
  eastl::unique_ptr<KeyState[]> keys_, prev_keys_;
  DirectX::XMINT2 mouse_movement_;
};

} // namespace Octane
