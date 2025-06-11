#version 120

varying vec4 v_position;

void main() {
	v_position = gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
