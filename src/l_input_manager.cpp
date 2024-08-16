#include "l_input_manager.h"

namespace lain
{
    namespace input_manager
    {
        static bool _currentKeys[static_cast<std::size_t>(key::count)];
        static bool _previousKeys[static_cast<std::size_t>(key::count)];
        static bool _previousMouseButtons[static_cast<std::size_t>(mouse_button::count)];
        static bool _currentMouseButtons[static_cast<std::size_t>(mouse_button::count)];
        static glm::vec2 _cursorCoords;
        static bool _cursorIsMoving;

        void Initialise()
        {
            for (std::size_t i{0}; i < static_cast<std::size_t>(key::count); ++i) {
                _currentKeys[i] = false;
                _previousKeys[i] = false;
            }

            for (std::size_t i{0}; i < static_cast<std::size_t>(mouse_button::count); ++i) {
                _previousMouseButtons[i] = false;
                _currentMouseButtons[i] = false;
            }
        }

        void BeginFrame()
        {
            for (std::size_t i{0}; i < static_cast<std::size_t>(key::count); ++i) {
                _previousKeys[i] = _currentKeys[i];
            }

            for (std::size_t i{0}; i < static_cast<std::size_t>(mouse_button::count); ++i) {
                _previousMouseButtons[i] = _currentMouseButtons[i];
            }

            _cursorIsMoving = false;
        }

        void UpdateKey(key const k, bool const pressed)
        {
            _currentKeys[static_cast<std::size_t>(k)] = pressed;
        }

        void UpdateMouseButton(mouse_button const button, bool const clicked)
        {
            _currentMouseButtons[static_cast<std::size_t>(button)] = clicked;
        }

        bool IsKeyPressed(key const k)
        {
            return !_previousKeys[static_cast<std::size_t>(k)] && _currentKeys[static_cast<std::size_t>(k)];
        }

        bool IsKeyHeld(key const k)
        {
            return _previousKeys[static_cast<std::size_t>(k)] && _currentKeys[static_cast<std::size_t>(k)];
        }

        bool IsKeyReleased(key const k)
        {
            return _previousKeys[static_cast<std::size_t>(k)] && !_currentKeys[static_cast<std::size_t>(k)];
        }

        bool IsMouseButtonPressed(mouse_button const button)
        {
            return _previousMouseButtons[static_cast<std::size_t>(button)] &&
                  !_currentMouseButtons[static_cast<std::size_t>(button)];
        }

        bool IsMouseButtonHeld(mouse_button const button)
        {
            return _previousMouseButtons[static_cast<std::size_t>(button)] &&
                   _currentMouseButtons[static_cast<std::size_t>(button)];
        }

        bool IsMouseButtonReleased(mouse_button const button)
        {
            return _previousMouseButtons[static_cast<std::size_t>(button)] &&
                   !_currentMouseButtons[static_cast<std::size_t>(button)];
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
