#version 460 core
out vec4 FragColour;
in  vec3 FragPos;

uniform int maxIters;

void main() {
	vec2 c = FragPos.xy;
	vec2 z = vec2(0.0f, 0.0f);
	int iters = 0;
	for (int i=0; i<maxIters; i++) {
		z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
		if (dot(z, z) > 4.0) {
			break;
		}
		iters = i;
	}
	float t;
	if (iters == maxIters-1.0f) {t = 1.0f;}
	else {t = float(iters %25)/25.0f;}
	FragColour = vec4(0.0f, t, t, 1.0f);
}