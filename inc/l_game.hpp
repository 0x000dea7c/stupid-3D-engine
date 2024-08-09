#pragma once

namespace lain {

namespace game {

void Initialise();

// -----------------------------------
// You use in game menu to shut down
// -----------------------------------
bool IsShuttingDown();

// -------------------------------------------------
// You use Ctrl+C, press X button in the window, etc
// --------------------------------------------------
void ForceShutdown();

void ProcessInput();

void Update(float const deltaTime);

void Render();

}; // namespace game

// class level_editor;
// class event_manager;
//   class main_menu;

// enum class game_state { mainMenu, play, editor, options, quit };

// class game final {
// public:
//   game(level_editor& levelEditor, event_manager& eventManager, main_menu& mainMenu);

//   bool IsDone() const { return _done; }

//   void Done() { _done = true; }

//   void ProcessInput(float const deltaTime);

//   void Update(float const deltaTime);

//   void Render();

// private:
//   void ChangeGameState(game_state const gameState);

//   std::stack<game_state> _state;
//   level_editor& _levelEditor;
//   event_manager& _eventManager;
//   main_menu& _mainMenu;
//   bool _done;
//   bool _developerMode;
// };

}; // namespace lain
