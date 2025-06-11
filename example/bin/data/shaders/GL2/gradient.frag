#version 120

uniform vec2 u_resolution;
uniform vec2 u_center;
uniform float u_innerRadius;
uniform float u_middleRadius;
uniform float u_outerRadius;

uniform vec3 u_colorInner;
uniform vec3 u_colorMiddle;
uniform vec3 u_colorOuter;

void main() {
	vec2 fragPos = gl_FragCoord.xy;

	float dist = distance(fragPos, u_center);
	vec3 color;

	if (dist <= u_innerRadius) {
		color = u_colorInner;
	} else if (dist <= u_middleRadius) {
		float t = smoothstep(u_innerRadius, u_middleRadius, dist);
		color = mix(u_colorInner, u_colorMiddle, t);
	} else if (dist <= u_outerRadius) {
		float t = smoothstep(u_middleRadius, u_outerRadius, dist);
		color = mix(u_colorMiddle, u_colorOuter, t);
	} else {
		color = u_colorOuter;
	}

	gl_FragColor = vec4(color, 1.0);

	//gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0); // hot pink

}