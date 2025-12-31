#version 460 core
out vec4 FragColour;
in  vec4 FragPos;

uniform int maxIters;
uniform vec3 c1;
uniform vec3 c2;
uniform int banding;

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
	if (iters == maxIters-1.0f) t = 1.0f;
	else t = float(iters % banding)/float(banding);
	float r = c1.x + t*(c2.x-c1.x);
	float g = c1.y + t*(c2.y-c1.y);
	float b = c1.z + t*(c2.z-c1.z);
	FragColour = vec4(r, g, b, 1.0f);
}