#version 150

uniform sampler2D gFontAtlas;

in vec2 vTexCoord;

out vec4 oFragColor;

void main() {
	oFragColor = texture(gFontAtlas, vTexCoord);
}