#version 150

uniform sampler2D gColorTexture;

in vec2 vTexCoord;
in vec4 vColor;

out vec4 oFragColor;

void main() {
	oFragColor = texture(gColorTexture, vTexCoord) * vColor;
}