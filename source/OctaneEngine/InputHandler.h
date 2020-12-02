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

  enum class MouseButton
  {
    LEFT = SDL_BUTTON_LEFT,
    MIDDLE = SDL_BUTTON_MIDDLE,
    RIGHT = SDL_BUTTON_RIGHT,

    COUNT = 4
  };

  bool MouseButtonPressed(MouseButton button);
  bool MouseButtonHeld(MouseButton button);
  bool MouseButtonPressedOrHeld(MouseButton button);
  bool MouseButtonReleased(MouseButton button);

  DirectX::XMINT2 GetMouseMovement();
  DirectX::XMINT2 GetMousePosition();

private:
  void ProcessAllSDLEvents();
  void HandleGlobalHotkeys();

  eastl::unique_ptr<KeyState[]> keys_, prev_keys_;
  eastl::unique_ptr<KeyState[]> mouse_buttons_, prev_mouse_buttons_;
  DirectX::XMINT2 mouse_movement_;
  DirectX::XMINT2 mouse_position_;
};

} // namespace Octane
