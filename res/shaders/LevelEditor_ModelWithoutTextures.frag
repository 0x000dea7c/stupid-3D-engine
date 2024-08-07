#version 460 core

out vec4 FragColour;

in vec2 texCoord;

uniform vec3 diffuseColour;

void main() {
  FragColour = vec4(diffuseColour, 1.f);
}
