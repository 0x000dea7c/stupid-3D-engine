#version 460 core

out vec4 FragColour;

in vec2 texCoord;

uniform sampler2D textureDiffuse1;

void main() {
  FragColour = texture(textureDiffuse1, texCoord);
}
