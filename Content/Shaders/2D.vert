#version 150

uniform mat4 gTransform;

in vec4 iPosition;
in vec2 iTexCoord;

out vec2 vTexCoord;

void main() {
	gl_Position = gTransform * iPosition;
	vTexCoord = iTexCoord;
}
