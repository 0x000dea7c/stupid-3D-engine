#include "l_game.h"
#include "l_event_manager.h"
#include "l_input_manager.h"
#include "l_level_editor.h"
#include "l_main_menu.h"
#include <stack>

namespace lain
{
    namespace game
    {
        enum class game_state
        {
            mainMenu,
            play,
            levelEditor,
            options
        };

        static std::stack<game_state> _state;
        static bool _isShuttingDown;

        void Initialise()
        {
            _isShuttingDown = false;

            event_manager::Subscribe(event_type::main_menu_click_play, [](event const&) {
                _state.push(game_state::play);
            });

            event_manager::Subscribe(event_type::main_menu_click_options, [](event const&) {
                _state.push(game_state::options);
            });

            event_manager::Subscribe(event_type::main_menu_click_level_editor, [](event const&) {
                _state.push(game_state::levelEditor);
            });

            event_manager::Subscribe(event_type::main_menu_click_quit, [](event const&) {
                ForceShutdown();
            });

            _state.push(game_state::mainMenu);

            level_editor::Initialise();
        }

        bool IsShuttingDown()
        {
            return _isShuttingDown;
        }

        void ForceShutdown()
        {
            _isShuttingDown = true;
        }

        void ProcessInput()
        {
            if (input_manager::IsKeyPressed(input_manager::key::esc)) {
                ForceShutdown();
                return;
            }

            switch (_state.top()) {
                case game_state::mainMenu:
                    main_menu::ProcessInput();
                    break;
                case game_state::play:
                    break;
                case game_state::levelEditor:
                    level_editor::ProcessInput();
                    break;
                case game_state::options:
                    break;
            }
        }

        void Update(float const deltaTime)
        {
            switch (_state.top()) {
                case game_state::mainMenu:
                    main_menu::Update();
                    break;
                case game_state::play:
                    break;
                case game_state::levelEditor:
                    level_editor::Update(deltaTime);
                    break;
                case game_state::options:
                    break;
            }
        }

        void Render()
        {
            switch (_state.top()) {
                case game_state::mainMenu:
                    main_menu::Render();
                    break;
                case game_state::play:
                    break;
                case game_state::levelEditor:
                    level_editor::Render();
                    break;
                case game_state::options:
                    break;
            }
        }

    };
};
