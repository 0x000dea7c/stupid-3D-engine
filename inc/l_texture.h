#pragma once

namespace lain
{
    struct texture final
    {
        texture(unsigned int const id, int const width, int const height, int const channels)
        :   _id{id},
            _width{width},
            _height{height},
            _channels{channels}
        {
        }

        unsigned int _id;
        int _width;
        int _height;
        int _channels;
    };
};
