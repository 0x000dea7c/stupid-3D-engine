#pragma once

#include "l_mesh.h"
#include <string>

namespace lain
{
    // --------------------------------------
    // Define posible models to load in here.
    // --------------------------------------
    enum class model_type
    {
        ball,
        maze
    };

    struct model final
    {
        std::vector<mesh> _meshes;
        std::string _directory;
    };
};
