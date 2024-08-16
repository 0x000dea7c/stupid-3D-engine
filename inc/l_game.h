#pragma once

namespace lain
{
    namespace game
    {
        void Initialise();

        bool IsShuttingDown();

        void ForceShutdown();

        void ProcessInput();

        void Update(float const deltaTime);

        void Render();
    };
};
