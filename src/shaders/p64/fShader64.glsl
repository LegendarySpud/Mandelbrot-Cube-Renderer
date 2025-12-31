#version 460 core
out vec4 FragColour;
flat in dvec3 FragPos;

uniform int maxIters;

void main() {
	dvec2 c = FragPos.xy;
	dvec2 z = dvec2(0.0lf, 0.0lf);
	int iters = 0;
	for (int i=0; i<maxIters; i++) {
		z = dvec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
		if (dot(z, z) > 4.0lf) {
			break;
		}
		iters = i;
	}
	float t;
	if (iters == maxIters-1.0f) {t = 1.0f;}
	else {t = float(iters %25)/25.0f;}
	FragColour = vec4(0.0f, t, t, 1.0f);
}