#version 460 core

in vec2 texCoord;

out vec4 FragColour;

uniform sampler2D image;

void main() {
  vec4 colour = texture(image, texCoord);

  if (colour.a < 1.f) {
    discard;
  }

  FragColour = colour;
}
