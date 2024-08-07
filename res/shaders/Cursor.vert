#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform vec2 cursorSize;
uniform vec2 cursorPos;

out vec2 texCoord;

void main() {
  vec2 pos = aPos * cursorSize + cursorPos;
  texCoord = aTexCoord;
  gl_Position = projection * vec4(pos, 0.f, 1.f);
}
