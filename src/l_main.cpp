#include "l_application.h"
#include <cstdlib>

using namespace lain;

int main()
{
    bool const fullscreen{true};

    if (!application::Initialise(fullscreen)) {
        application::Shutdown();
        return EXIT_FAILURE;
    }

    application::Run();

    application::Shutdown();

    return EXIT_SUCCESS;
}
