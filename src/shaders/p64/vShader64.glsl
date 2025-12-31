#version 460 core
layout (location=0) in vec3 aPos;

flat out dvec3 FragPos;
uniform double ratio;
uniform double zoom;
uniform dvec2 pos;

double zoomVal = 1.25lf;

void main() {
	double z = zoom;
	FragPos = aPos*z+dvec3(pos, 1.0lf);
	FragPos.x *= ratio;
	gl_Position = vec4(aPos, 1.0lf);
}