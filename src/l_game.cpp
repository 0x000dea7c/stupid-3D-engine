#include "l_game.hpp"
#include "l_event_manager.hpp"
#include "l_input_manager.hpp"
#include "l_level_editor.hpp"
#include "l_main_menu.hpp"
#include <stack>

namespace lain {

namespace game {

enum class game_state { mainMenu, play, levelEditor, options };

static std::stack<game_state> _state;
static bool _isShuttingDown;

void Initialise() {
  _isShuttingDown = false;

  event_manager::Subscribe(event_type::main_menu_click_play,
                           [](event const&) { _state.push(game_state::play); });

  event_manager::Subscribe(event_type::main_menu_click_options,
                           [](event const&) { _state.push(game_state::options); });

  event_manager::Subscribe(event_type::main_menu_click_level_editor,
                           [](event const&) { _state.push(game_state::levelEditor); });

  event_manager::Subscribe(event_type::main_menu_click_quit, [](event const&) { ForceShutdown(); });

  _state.push(game_state::mainMenu);

  level_editor::Initialise();
}

bool IsShuttingDown() { return _isShuttingDown; }

void ForceShutdown() { _isShuttingDown = true; }

void ProcessInput() {
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

void Update(float const deltaTime) {
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

void Render() {
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

}; // namespace game

// game::game(level_editor& levelEditor, event_manager& eventManager, main_menu& mainMenu)
//     : _levelEditor{levelEditor},
//       _eventManager{eventManager},
//       _mainMenu{mainMenu},
//       _done{false},
//       _developerMode{true} {
//   ChangeGameState(game_state::mainMenu);

//   _eventManager.Subscribe(event_type::main_menu_click_play,
//                           [this](event const& /*event*/) { ChangeGameState(game_state::play); });

//   _eventManager.Subscribe(event_type::main_menu_click_level_editor,
//                           [this](event const& /*event*/) { ChangeGameState(game_state::editor);
//                           });

//   _eventManager.Subscribe(event_type::main_menu_click_options,
//                           [this](event const& /*event*/) { ChangeGameState(game_state::options);
//                           });

//   _eventManager.Subscribe(event_type::main_menu_click_quit,
//                           [this](event const& /*event*/) { ChangeGameState(game_state::quit); });
// }

// void game::ProcessInput(float const deltaTime) {
//   switch (_state.top()) {
//   case game_state::mainMenu:
//     break;
//   case game_state::play:
//     break;
//   case game_state::editor:
//     _levelEditor.ProcessInput(deltaTime);
//     break;
//   case game_state::options:
//     break;
//   case game_state::quit:
//     break;
//   }
// }

// void game::Update(float const deltaTime) {
//   switch (_state.top()) {
//   case game_state::mainMenu:
//     break;
//   case game_state::play:
//     break;
//   case game_state::editor:
//     _levelEditor.Update(deltaTime);
//     break;
//   case game_state::options:
//     break;
//   case game_state::quit:
//     Done();
//     break;
//   }
// }

// void game::Render() {
//   switch (_state.top()) {
//   case game_state::mainMenu:
//     _mainMenu.Render();
//     break;
//   case game_state::play:
//     break;
//   case game_state::editor:
//     _levelEditor.Render();
//     break;
//   case game_state::options:
//     break;
//   case game_state::quit:
//     break;
//   }
// }

// // TODO(luis): change name or add a bool to know if you're adding or deleting
// void game::ChangeGameState(game_state const gameState) { _state.push(gameState); }

}; // namespace lain
