#include "l_application.hpp"
#include <cstdlib>

using namespace lain;

int main() {
  if (!application::Initialise(1920.f, 1080.f, false)) {
    application::Shutdown();
    return EXIT_FAILURE;
  }

  application::Run();

  application::Shutdown();

  return EXIT_SUCCESS;
}
