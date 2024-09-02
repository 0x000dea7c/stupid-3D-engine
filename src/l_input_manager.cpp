#include "l_input_manager.h"

namespace lain
{
  namespace input_manager
  {
    static bool _currentKeys[static_cast<u32>(key::count)];
    static bool _previousKeys[static_cast<u32>(key::count)];
    static bool _previousMouseButtons[static_cast<u32>(mouse_button::count)];
    static bool _currentMouseButtons[static_cast<u32>(mouse_button::count)];
    static glm::vec2 _cursorCoords;
    static bool _cursorIsMoving;

    void Initialise()
    {
      for (u32 i{0}; i < static_cast<u32>(key::count); ++i) {
	_currentKeys[i] = false;
	_previousKeys[i] = false;
      }

      for (u32 i{0}; i < static_cast<u32>(mouse_button::count); ++i) {
	_previousMouseButtons[i] = false;
	_currentMouseButtons[i] = false;
      }
    }

    void BeginFrame()
    {
      for (u32 i{0}; i < static_cast<u32>(key::count); ++i) {
	_previousKeys[i] = _currentKeys[i];
      }

      for (u32 i{0}; i < static_cast<u32>(mouse_button::count); ++i) {
	_previousMouseButtons[i] = _currentMouseButtons[i];
      }

      _cursorIsMoving = false;
    }

    void UpdateKey(key k, bool pressed)
    {
      _currentKeys[static_cast<u32>(k)] = pressed;
    }

    void UpdateMouseButton(mouse_button button, bool clicked)
    {
      _currentMouseButtons[static_cast<u32>(button)] = clicked;
    }

    bool IsKeyPressed(key k)
    {
      return !_previousKeys[static_cast<u32>(k)] && _currentKeys[static_cast<u32>(k)];
    }

    bool IsKeyHeld(key k)
    {
      return _previousKeys[static_cast<u32>(k)] && _currentKeys[static_cast<u32>(k)];
    }

    bool IsKeyReleased(key k)
    {
      return _previousKeys[static_cast<u32>(k)] && !_currentKeys[static_cast<u32>(k)];
    }

    bool IsMouseButtonPressed(mouse_button button)
    {
      return _previousMouseButtons[static_cast<u32>(button)] &&
	!_currentMouseButtons[static_cast<u32>(button)];
    }

    bool IsMouseButtonHeld(mouse_button button)
    {
      return _previousMouseButtons[static_cast<u32>(button)] &&
	_currentMouseButtons[static_cast<u32>(button)];
    }

    bool IsMouseButtonReleased(mouse_button button)
    {
      return _previousMouseButtons[static_cast<u32>(button)] &&
	!_currentMouseButtons[static_cast<u32>(button)];
    }

    bool IsCursorMoving()
    {
      return _cursorIsMoving;
    }

    void UpdateCursorPosition(glm::vec2 const& pos)
    {
      _cursorCoords = pos;
    }

    void SetCursorIsMoving()
    {
      _cursorIsMoving = true;
    }

    glm::vec2 GetCursorPosition()
    {
      return _cursorCoords;
    }
  };
};
