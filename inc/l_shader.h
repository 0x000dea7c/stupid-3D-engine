#pragma once

namespace lain
{
  struct shader final
  {
    shader() = default;

    shader(u32 id,
	   u32 vao,
	   u32 vbo)
      :   _id{id},
	  _vao{vao},
	  _vbo{vbo}
    {
      _ebo = 0;
      _ubo[0] = _ubo[1] = 0;
    }

    shader(u32 id,
	   u32 vao,
	   u32 vbo,
	   u32 ebo)
      :   _id{id},
	  _vao{vao},
	  _vbo{vbo},
	  _ebo{ebo}
    {
      _ubo[0] = _ubo[1] = 0;
    }

    shader(u32 id)
      :   _id{id},
	  _vao{0},
	  _vbo{0},
	  _ebo{0}
    {
      _ubo[0] = _ubo[1] = 0;
    }

    u32 _id;
    u32 _vao;
    u32 _vbo;
    u32 _ebo;
    u32 _ubo[2];
  };

  enum class shader_type
    {
      vertex,
      fragment,
      program
    };
};
