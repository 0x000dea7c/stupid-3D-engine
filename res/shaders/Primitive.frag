#version 460 core

out vec4 FragColour;

uniform vec4 colour;

// TODO: change colour's alpha depending on its distance to the camera!
void main() {
  FragColour = colour;
}
