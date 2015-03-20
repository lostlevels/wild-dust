#version 150

uniform mat4 gTransform;

in vec4 iPosition;
in vec2 iTexCoord;
in vec4 iColor;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
	gl_Position = gTransform * iPosition;
	vTexCoord = iTexCoord;
	vColor = iColor;
}
