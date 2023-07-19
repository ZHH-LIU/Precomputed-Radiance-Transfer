#version 330 core

in vec3 color;
in vec3 glossy;
out vec4 fragColor;

void main()
{
	fragColor = vec4(glossy+color,1.0);
}