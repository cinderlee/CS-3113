
uniform vec4 color;
uniform float alpha;

void main() {
    float a = alpha;
    if (a > 1.0) {
        a = 1.0;
    }
    if (a < 0.0) {
        a = 0.0;
    }
	gl_FragColor = vec4 (color.r, color.b, color.g, a);
}
