#version 460 core
layout (location=0) in vec3 aPos;

out vec4 FragPos;
uniform float zoom;
uniform vec2 pos;
uniform mat4 mat;
uniform mat4 effectMat;

void main() {
	//FragPos = vec4(aPos, 1.0f)*zoom*effectMat+vec4(pos, 0.0f, 1.0f);
	FragPos = zoom*effectMat*vec4(aPos, 1.0f) + vec4(pos, 0.0f, 1.0f);
	gl_Position = mat*vec4(aPos, 1.0f);
}