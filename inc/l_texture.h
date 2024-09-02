#pragma once

namespace lain
{
  struct texture final
  {
    texture(u32 id, i32 width, i32 height, i32 channels)
      :   _id{id},
	  _width{width},
	  _height{height},
	  _channels{channels}
    {
    }

    u32 _id;
    i32 _width;
    i32 _height;
    i32 _channels;
  };
};
