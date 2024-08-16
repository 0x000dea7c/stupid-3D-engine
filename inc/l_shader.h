#pragma once

namespace lain
{
    struct shader final
    {
        shader() = default;

        shader(unsigned int const id,
               unsigned int const vao,
               unsigned int const vbo)
        :   _id{id},
            _vao{vao},
            _vbo{vbo}
        {
            _ebo = 0;
            _ubo[0] = _ubo[1] = 0;
        }

        shader(unsigned int const id,
               unsigned int const vao,
               unsigned int const vbo,
               unsigned int const ebo)
        :   _id{id},
            _vao{vao},
            _vbo{vbo},
            _ebo{ebo}
        {
            _ubo[0] = _ubo[1] = 0;
        }

        shader(unsigned int const id)
        :   _id{id},
            _vao{0},
            _vbo{0},
            _ebo{0}
        {
            _ubo[0] = _ubo[1] = 0;
        }

        unsigned int _id;
        unsigned int _vao;
        unsigned int _vbo;
        unsigned int _ebo;
        unsigned int _ubo[2];
    };

    enum class shader_type
    {
        vertex,
        fragment,
        program
    };
};
