#pragma once

// ------------------------------------------------------------------------------
// Platform specific code that is needed across the game goes here (declarations)
// Implementations go in their own .cpp file
// ------------------------------------------------------------------------------
namespace lain {

// -------------------------------------------------
// Makes the mouse move only inside the window
// -------------------------------------------------
void ConstrainCursorInWindow();

// -------------------------------------------------
// Undoes what the `ConstrainCursorInWindow` did
// -------------------------------------------------
void ReleaseCursorFromWindow();

}; // namespace lain
