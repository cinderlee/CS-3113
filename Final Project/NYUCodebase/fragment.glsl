
uniform vec4 color;
uniform float alpha;

void main() {
	gl_FragColor = vec4 (color.r, color.b, color.g, alpha);
}
