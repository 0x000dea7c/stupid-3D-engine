#pragma once

#include "glm/ext/vector_float2.hpp"
#include <stack>

namespace lain {

class level_editor;
class event_manager;
  class main_menu;

enum class game_state { mainMenu, play, editor, options, quit };

class game final {
public:
  game(level_editor& levelEditor, event_manager& eventManager, main_menu& mainMenu);

  bool IsDone() const { return _done; }

  void Done() { _done = true; }

  void ProcessInput(float const deltaTime);

  void Update(float const deltaTime);

  void Render(glm::vec2 const& mousePos);

private:
  void ChangeGameState(game_state const gameState);

  std::stack<game_state> _state;
  level_editor& _levelEditor;
  event_manager& _eventManager;
  main_menu& _mainMenu;
  bool _done;
  bool _developerMode;
};

}; // namespace lain
