#version 330
// Output color
layout(location = 0) out vec4 color;
in vec2 TexCoords;
void main()
{
	color = vec4(sin(TexCoords.x*9.0),sin(TexCoords.y*9.0),cos((TexCoords.x+TexCoords.y)*9.0), 1.0);
}